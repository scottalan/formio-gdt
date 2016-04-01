<?php

/**
 * Settings form used to collect configuration used to communicate with Formio.
 *
 * @return array
 *   System settings form.
 */
function formio_settings_form() {
  $form = array();
  $form['formio'] = array(
    '#title' => t('Settings'),
    '#type' => 'fieldset',
  );
  $form['formio']['formio_project_url'] = array(
    '#title' => t('Project URL'),
    '#type' => 'textfield',
    '#description' => t('Enter your Form.io project URL'),
    '#default_value' => variable_get('formio_project_url', ''),
  );
  $form['formio']['formio_project_api_key'] = array(
    '#title' => t('Project API Key'),
    '#type' => 'textfield',
    '#description' => t('Enter your Form.io project API key'),
    '#default_value' => variable_get('formio_project_api_key', ''),
  );

  return system_settings_form($form);
}

/**
 * Preset form used to collect information about actions Formio will perform.
 *
 * @return array
 *   Form containing preset selections.
 */
function formio_preset_form() {
  $form = array();
  $form_state = array();
  $options = array();

  if (formio_get_project()) {
    $formios = formio_get_forms();

    $options['_none'] = t('Forms');
    foreach ($formios as $name => $values) {
      $options[$values['path']] = $values['title'];
    }

    // Placeholder for rendered formio form.
    $form['formio']['formio_render_form'] = array(
      '#markup' => '<div id="formio-forms"></div>',
    );

    $form['formio']['formio_preset_select'] = array(
      '#title' => t('Select Form'),
      '#type' => 'select',
      '#options' => $options,
      '#description' => t('Choose a form'),
      '#default_value' => variable_get('formio_preset_select', '_none'),
      '#ajax' => array(
        'callback' => 'formio_preset_select_ajax',
        'wrapper' => 'formio-forms',
        'method' => 'replace',
      ),
    );

    $form['formio']['formio_preset_action'] = array(
      '#title' => t('Action'),
      '#type' => 'select',
      '#options' => array(
        '_none' => t('Choose'),
        'entity_create' => t('Create Entity'),
      ),
      '#default_value' => variable_get('formio_preset_action', '_none'),
      '#states' => array(
        'invisible' => array(
          ':input[name="formio_preset_select"]' => array('value' => '_none'),
        ),
      ),
      '#ajax' => array(
        'callback' => 'formio_preset_action_ajax',
        'wrapper' => 'formio-preset-action',
        'method' => 'after',
      ),
    );

    // Add a selection for none.
    $names['_none'] = t('Choose content type');
    // Get all content types.
    $names += node_type_get_names();

    $form['formio']['formio_entity_type'] = array(
      '#title' => t('Entity Type'),
      '#type' => 'select',
      '#options' => $names,
      '#default_value' => variable_get('formio_entity_type', '_none'),
      '#states' => array(
        'visible' => array(
          ':input[name="formio_preset_action"]' => array('value' => 'entity_create'),
        ),
      ),
      '#ajax' => array(
        'callback' => 'formio_preset_entity_type_form_ajax',
        'method' => 'after',
      ),
    );

    $form['formio']['map_fields'] = array(
      '#title' => t('Map Fields'),
      '#type' => 'fieldset',
      '#states' => array(
        'visible' => array(
          ':input[name="formio_preset_action"]' => array('value' => 'entity_create'),
        ),
      ),
    );

    return system_settings_form($form);
  }
}

/**
 * Ajax callback used to render Formio forms.
 *
 * @param array $form
 *   Form that triggered the callback.
 * @param array $form_state
 *   State of the form.
 *
 * @return array
 *   Ajax commands.
 */
function formio_preset_select_ajax($form, &$form_state) {
 	$commands = array();
  // The form to target via the api.
  $formio = $form_state['values']['formio_preset_select'];
  // Render the form.
  $block = module_invoke('formio', 'block_view', 'formio_preset', $formio);
  // Render the html in the div we created.
	$commands[] = ajax_command_html('#formio-forms', $block['content']);
  $commands[] = ajax_command_invoke('#content', 'fadeIn', array('fast'));

	// The command is a javascript function and the others are arguments
	$commands[] = array(
    'command' => 'doSomething',
    'form' => $formio,
    'api' => formio_get_project(),
  );

	// Return commands to the system to be executed.
	return array('#type' => 'ajax', '#commands' => $commands);
}

function formio_preset_action_ajax(&$form, &$form_state) {
  $action = $form_state['values']['formio_preset_action'];
  switch ($action) {
    case 'entity_create':
      return $form['formio']['formio_entity_type'];
//      formio_preset_entity_type_form($form, $form_state);
      break;
    case '_none':
//      formio_preset_field_map_form($form, $form_state);
      break;
  }
}

function formio_preset_entity_type_form_ajax(&$form, &$form_state) {
  $type = $form_state['values']['formio_entity_type'];
  $formios = formio_get_forms();
  foreach ($formios as $formio_name => $data) {
    $options[$data['path']] = $data['title'];
  }
  $fields = field_info_instances('node', $type);
  foreach ($fields as $name => $info) {
    $form['formio']['map_fields']['field_' . $info['bundle'] . '_' . $info['entity_type'] . '_' . $info['field_name']] = array(
      '#title' => t('@name', array('@name' => $name)),
      '#type' => 'select',
      '#options' => $options,
//      '#states' => array(
//        'visible' => array(
//          ':input[name="formio_entity_type"]' => array('filled' => TRUE),
//        ),
//      ),
    );
  }
  $form_state['rebuild'] = TRUE;
}

function formio_preset_field_map_form_ajax(&$form, &$form_state) {

}

function formio_preset_form_submit($form) {
  if ($form) {

  }
}
