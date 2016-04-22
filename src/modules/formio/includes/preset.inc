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
function formio_preset_choose_form($form, &$form_state) {

  ctools_form_include($form_state, 'preset', 'formio');

  $formio = formio_get_forms();
  $forms = $formio->fetchOptions();

  if (!empty($forms)) {

    $options = array();
    $options['_none'] = t('Choose your form...');
    foreach ($forms as $id => $data) {
      $options[$id] = $data['title'];
    }

    $form += formio_preset_display_form($form, $form_state);

    $form['formio_preset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Preset Options'),
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
        'callback' => 'formio_preset_choose_ajax',
        'wrapper' => 'formio-form-render',
        'path' => 'system/formio_ajax',
        'method' => 'replace',
      ),
    );
  }
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
function formio_preset_choose_ajax($form, $form_state) {
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
 * ACTION FORM.
 */
function formio_preset_action_form($form, &$form_state) {
  $form['formio_preset']['formio_action'] = array(
    '#title' => t('Action'),
    '#type' => 'select',
    '#options' => formio_preset_get_actions(),
    '#default_value' => isset($form_state['item']->action) ? $form_state['item']->action : '',
  );

  return $form;
}

function formio_preset_action_form_validate($form, &$form_state) {
  if ($form_state['values']['formio_action'] == '_none') {
    form_set_error('formio_action', t('You must choose an Action'));
  }
}

function formio_preset_action_form_submit($form, &$form_state) {
  if ($form) {

  }
}

function formio_preset_configure_form($form, &$form_state) {
  // The configuration form will be dependent on the action that was selected.
  $hash = $form_state['item']->action;
  // Actions base64 encode the function names so we need to do a lookup.
  // @see actions_actions_map().
  $aid = actions_function_lookup($hash);
  // Build a form callback using the callback stored in the {actions} table.
  $function = 'formio_' . $aid . '_form';
  // We need to pass by reference so not using module_invoke_all().
  foreach (module_implements($function) as $module) {
    $callback = $module . '_' . $function;
    $form = $callback($form, $form_state);
  }

  if (!isset($callback)) {
    $form['no_configure'] = array(
      '#markup' => t('There is nothing to configure for this action.'),
    );
  }
  return $form;
}

/**
 * Ajax callback.
 *
 * Actions can be anything therefore we need a way to generate the form element
 * based on the option that was selected. We use the 'key' of the option to
 * to build the hook we want to invoke.
 *
 * @param array $form
 *   Form that triggered the callback.
 * @param array $form_state
 *   State of the form.
 */
//function formio_form_action_ajax($form, $form_state, $args) {
//  $action = $form_state['values']['formio_action'];
//  if ($action != '_none') {
//    $hook = 'io_preset_action_render__' . $action;
//    foreach (module_implements($hook) as $module) {
//      $function = $module . '_' . $hook;
//      // Expects an array of ajax commands.
//      $commands = $function($form, $form_state);
//      return $commands;
//    }
//  }
//}

/**
 * Override the submit handler for our preset.
 *
 * @param $form
 * @param $form_state
 */
//function formio_preset_form_submit($form, &$form_state) {
//  // Transfer data from the form to the $item based upon schema values.
//  $schema = ctools_export_get_schema($form_state['plugin']['schema']);
//
//  // Set all the default items and any others that match the key.
//  foreach (array_keys($schema['fields']) as $key) {
//    if(isset($form_state['values'][$key])) {
//      $form_state['item']->{$key} = $form_state['values'][$key];
//    }
//  }
//
//  // Set our custom values.
//  $form_state['item']->id = $form_state['values']['formio_form'];
//  $form_state['item']->action = $form_state['values']['formio_action'];
//  $form_state['item']->data = array();
//}

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
    $formio = new FormioRequest();
    $formio
      ->setOp()
      ->endpoint('form')
      ->setParams(array('type' => 'form'))
      ->request();
  }

  return $formio;
}

/**
 * @return array
 */
function formio_preset_get_actions() {
  $actions = actions_list();
  $actions_map = actions_actions_map($actions);
  $options = array();
  foreach ($actions_map as $key => $array) {
    if (!$array['configurable'] && $array['type'] == 'formio') {
      $options[$key] = $array['label'] . '...';
    }
  }
  // Add a '_none' option to the beginning of the options array.
  $options = array('_none' => t('Choose Action...')) + $options;
  return $options;
}
