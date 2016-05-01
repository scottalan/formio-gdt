<?php
/**
 * @file
 */

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
 * Preset form used to collect information about actions Formio will perform.
 *
 * @return array
 *   Form containing preset selections.
 */
function formio_add_formio_preset_choose_edit_form(&$form, &$form_state) {
  drupal_add_library('formio', 'seamless');

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
    $form['formio_preset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Preset Options !button', array('!button' => $html)),
    );
    $id = drupal_html_id('formio-preset-form');
    $form['formio_preset']['formio_form'] = array(
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

function formio_add_formio_preset_choose_edit_form_submit(&$form, &$form_state) {
  $form_state['item']->id = $form_state['values']['formio_form'];
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
  $args = array('formio_embed' => isset($form_state['item']->id) ? $form_state['item']->id : $form_state['values']['formio_form']);
  // Render the form block.
  $block = module_invoke('formio', 'block_view', 'formio_form', $args);

  // Render the html in the Formio fieldset. @todo: Fix this render
  $content = theme('formio_form', $block['content']);
  $commands[] = ajax_command_html('#formio-form-render', $content);

  // Return commands to the system to be executed.
  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * @param $form
 * @param $form_state
 */
function formio_add_formio_preset_action_edit_form(&$form, &$form_state) {

  if (isset($form_state['item']->action)) {
    $key = drupal_hash_base64($form_state['item']->action);
    $default_value = $key;
  }

  $form['formio_preset']['formio_action'] = array(
    '#title' => t('Action'),
    '#type' => 'select',
    '#options' => formio_preset_get_actions(),
    '#default_value' => isset($default_value) ? $default_value : '',
  );
}

/**
 * ACTION FORM.
 */
function formio_preset_action_form($form, &$form_state) {

  if (isset($form_state['item']->action)) {
    $key = drupal_hash_base64($form_state['item']->action);
    $default_value = $key;
  }

  $form['formio_preset']['formio_action'] = array(
    '#title' => t('Action'),
    '#type' => 'select',
    '#options' => formio_preset_get_actions(),
    '#default_value' => isset($default_value) ? $default_value : '',
  );

  return $form;
}

function formio_preset_action_form_validate($form, &$form_state) {
  if ($form_state['values']['formio_action'] == '_none') {
    form_set_error('formio_action', t('You must choose an Action'));
  }
}

function formio_add_formio_preset_action_edit_form_submit(&$form, &$form_state) {
  // The configuration form will be dependent on the action that was selected.
  $hash = $form_state['values']['formio_action'];
  // Actions base64 encode the function names so we need to do a lookup.
  // @see actions_actions_map().
  $aid = actions_function_lookup($hash);

  // Store the value as the function name, not the hash so we actually know
  // what it is.
  $form_state['item']->action = $aid;
}

function formio_preset_action_form_submit($form, &$form_state) {
  // The configuration form will be dependent on the action that was selected.
  $hash = $form_state['values']['formio_action'];
  // Actions base64 encode the function names so we need to do a lookup.
  // @see actions_actions_map().
  $aid = actions_function_lookup($hash);

  $form_state['item']->action = $aid;
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
function formio_add_formio_preset_settings_edit_form(&$form, &$form_state) {
  $text = t("If you don't see anymore steps you are done. Please continue or click Finish to save your preset.");
  $form['pre_settings'] = array(
    '#markup' => '<div class="pre-settings">' . $text . '</div>'
  );
}


// Edit forms //


function formio_edit_formio_preset_choose_edit_form(&$form, &$form_state) {
  formio_add_formio_preset_choose_edit_form($form, $form_state);
}

function formio_edit_formio_preset_choose_edit_form_submit(&$form, &$form_state) {
  formio_add_formio_preset_choose_edit_form_submit($form, $form_state);
}

function formio_edit_formio_preset_action_edit_form(&$form, &$form_state) {
  formio_add_formio_preset_action_edit_form($form, $form_state);
}

function formio_edit_formio_preset_action_edit_form_submit(&$form, &$form_state) {
  formio_add_formio_preset_action_edit_form_submit($form, $form_state);
}

function formio_edit_formio_preset_settings_edit_form(&$form, &$form_state) {
  formio_add_formio_preset_settings_edit_form($form, $form_state);
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

/**
 * @return array
 */
function formio_preset_get_actions() {
  $actions_map = actions_actions_map(actions_list());
  $options = array();
  foreach ($actions_map as $key => $array) {
    if ($array['type'] == 'formio') {
      $options[$key] = $array['label'] . '...';
    }
  }
  // Add a '_none' option to the beginning of the options array.
//  $options = array('_none' => t('Choose Action...')) + $options;
  return $options;
}
