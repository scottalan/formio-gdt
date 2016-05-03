<?php

/**
 * Defines a ctools Export UI plugin for Form IO presets.
 */

$plugin = array(
  // As defined in hook_schema().
  'schema' => FORMIO_PRESET,
  // Define a permission users must have to access these pages.
  'access' => 'administer formio',

  // Define the menu item.
  'menu' => array(
    // The 'menu prefix' and 'menu item' create the base path. The default path
    // is 'admin/structure/'. This will give us 'admin/structure/formio/preset',
    // 'admin/structure/formio/preset/add', etc.
    // @see ctools_export_ui_plugin_base_path().
    'menu item' => 'formio',
    'menu title' => 'Form.io',
    'menu description' => 'Form.io preset management.',
    'items' => array(),
  ),

  // Define user interface texts.
  'title singular' => t('Form.io preset'),
  'title plural' => t('Form.io presets'),
  'title singular proper' => t('Form.io Preset'),
  'title plural proper' => t('Form.io Presets'),

  'handler' => array(
    'class' => 'FormioPresetUI',
    'parent' => 'ctools_export_ui',
  ),

  'strings' => array(
    'message' => array(
      'formio' => t('This is a message from Form.io!'),
    ),
  ),

  'use wizard' => TRUE,
  'form info' => array(
    'id' => FORMIO_PRESET,
    'add order' => array(
      'admin' => t('Give your preset a name'),
      'choose' => t('Choose a form'),
      'action' => t('Select an action'),
      'settings' => t('Configure Settings'),
    ),
    'order' => array(
      'choose' => t('Choose a form'),
      'action' => t('Select an action'),
      'settings' => t('Configure Settings'),
    ),
    'forms' => array(),
  ),

  'default steps' => formio_preset_defaults(),

  'allowed operations' => array(),
);

/**
 * Gets the default forms used for this preset.
 *
 * @return array
 *   The default forms for this preset.
 */
function formio_preset_defaults() {
  include_once DRUPAL_ROOT . '/includes/common.inc';
  return drupal_map_assoc(array('choose', 'action', 'settings'));
}

/**
 * Renders the form when creating a preset.
 *
 * @param array $form
 * @param array $form_state
 * @return array
 */
function formio_preset_display_form($form, &$form_state) {
  // Placeholder for rendered formio form.
  $form['formio_render'] = array(
    '#type' => 'fieldset',
    '#title' => t('Form.io'),
  );
  $form['formio_render']['formio_display'] = array(
    '#markup' => '<div id="formio-form-render"></div>',
  );

  return $form;
}

/**
 * Ajax callback.
 *
 * Renders the Form IO form that is selected.
 *
 * @param array $form
 *   Form that triggered the callback.
 * @param array $form_state
 *   State of the form.
 *
 * @return array
 *   Ajax commands.
 */
function formio_preset_display_ajax($form, $form_state) {
  $commands = array();
  // The form to target via the api.
  $args = array(
    'form' => isset($form_state['item']->id) ?
      $form_state['item']->id :
      $form_state['values']['formio_form'],
    'preset' => TRUE,
  );
  // Render the form block.
  $block = module_invoke('formio', 'block_view', 'formio_form', $args);

  // Render the html in the Formio fieldset. @todo: Fix this render
  $content = theme('formio_form', $block['content']);
  $commands[] = ajax_command_html('#formio-form-render', $content);

  // Return commands to the system to be executed.
  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Preset form used to collect information about actions Formio will perform.
 *
 * @return array
 *   Form containing preset selections.
 */
function formio_preset_choose_form(&$form, &$form_state) {
//  drupal_add_library('formio', 'seamless');

  if ($cache = cache_get(__FUNCTION__ . 'all_forms')) {
    $forms = $cache->data;
  }
  else {
    $request = formio_rest('form', array('type' => 'form'));
    $forms = $request->fetchOptions();
    cache_set(__FUNCTION__ . 'all_forms', $forms);
  }

  if (!empty($forms)) {

    $options = array();
    $options['_none'] = t('Choose your form...');
    foreach ($forms as $id => $data) {
      $options[$id] = $data['title'];
    }

    $form += formio_preset_display_form($form, $form_state);

    $html = '<button type="button" id="reload-preset" class="btn btn-info btn-sm">reload</button>';
    $form[FORMIO_PRESET] = array(
      '#type' => 'fieldset',
      '#title' => t('Preset Options !button', array('!button' => $html)),
    );
    $id = drupal_html_id('formio-preset-form');
    $form[FORMIO_PRESET]['formio_form'] = array(
      '#title' => t('Form.io Form'),
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => isset($form_state['item']->id) ? $form_state['item']->id : '',
      '#prefix' => '<div id="' . $id . '">',
      '#suffix' => '</div>',
      '#ajax' => array(
        'callback' => 'formio_preset_display_ajax',
        'wrapper' => 'formio-form-render',
        'path' => 'system/formio_ajax',
        'method' => 'replace',
      ),
      '#attached' => array(
        'library' => array(
          array('formio', 'seamless'),
        ),
        'js' => array(
          drupal_get_path('module', 'formio') . '/js/formio.js' => array(NULL, 'module'),
        ),
      ),
    );
  }
}

/**
 * Submission function for the entity_type selection step.
 *
 * @param $form
 * @param $form_state
 */
function formio_preset_choose_form_submit(&$form, &$form_state) {
  $form_state['item']->id = $form_state['values']['formio_form'];
}

/**
 * Step used to choose an action to perform when the form is submitted.
 *
 * @param $form
 * @param $form_state
 */
function formio_preset_action_form(&$form, &$form_state) {

  // Get all plugin actions.
  $actions = formio_preset_get_actions();

  $default = '';
  if (isset($form_state['item']->action)) {
    list(, $default) = explode(':', $form_state['item']->action);
  }

  $form[FORMIO_PRESET]['formio_action'] = array(
    '#title' => t('Action'),
    '#type' => 'select',
    '#options' => $actions,
    '#default_value' => $default,
  );
}

/**
 * Submission for the action step.
 * @param $form
 * @param $form_state
 */
function formio_preset_action_form_submit(&$form, &$form_state) {
  // Store the value in the format of $module:$plugin.
  $form_state['item']->action = $form_state['plugin']['module'] . ':' . $form_state['values']['formio_action'];
}

/**
 * This part of the preset wizard is where we determine what configuration needs
 * to be exported with the preset based on the action selected in the previous
 * step.
 *
 * An action must created and stored in the database. We build a callback used
 * to render form based on the name of the action.
 *
 * An action with an {aid} of 'formio_actions_post_action' should render a form
 * using a callback with the naming convention of:
 *   - formio_formio_actions_post_action_form().
 *
 * If a module implements this function returning a form, this form will be
 * used in this step of the preset.
 */
function formio_preset_settings_form(&$form, &$form_state) {
  if (!empty($form_state['item']->settings)) {
    $text = t("This Action requires a few more settings. Keep going you're almost done!");
  }
  else {
    $text = t("This Action might require a few more settings, if not click Finish and you're done!");
  }
  $form['pre_settings'] = array(
    '#prefix' => '<div class="preset-settings">',
    '#markup' => $text,
    '#suffix' => '</div>',
  );
}

/**
 * @return array
 */
function formio_preset_get_actions() {
  $actions = array();
  ctools_include('action', 'formio');
  $plugins = formio_get_actions();
  foreach ($plugins as $id => $plugin) {
    if ($plugin['has actions']) {
      $handler = formio_actions_get_handler($plugin);
      if ($handler) {
        $method = method_exists($handler, 'action');
        if ($method) {
          $actions[$plugin['name']] = $plugin['title'];
        }
      }
      else {
        $function = ctools_plugin_get_function($plugin, 'action');
        if (function_exists($function)) {
          $actions[$plugin['name']] = $plugin['title'];
        }
      }
    }
  }
  return $actions;
}







/**
 * Forms used for preset.
 *
 * @param bool $reset
 * @return array
 */
function formio_get_forms($reset = FALSE) {
  static $drupal_static_fast;
  if (!isset($drupal_static_fast)) {
    $drupal_static_fast = &drupal_static(__FUNCTION__);
  }
  $formio = &$drupal_static_fast;

  if ($reset || !isset($formio)) {
    $formio = formio_rest('form', array('type' => 'form'));
  }

  return $formio;
}
