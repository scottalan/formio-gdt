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
function formio_presets_form() {
  $form = array();
  $form_state = array();
  $options = array();

  if (formio_get_project()) {
    $formio_forms = formio_get_forms();

    $options['_none'] = t('Forms');
    foreach ($formio_forms as $name => $values) {
      $options[$values['path']] = $values['title'];
    }

    $form['formio']['formio_render_form'] = array(
      '#markup' => '<div id="formio-form-display"></div>',
    );

    $form['formio']['formio_presets_select'] = array(
      '#title' => t('Select Form'),
      '#type' => 'select',
      '#options' => $options,
      '#description' => t('Choose a form'),
      '#default_value' => variable_get('formio_presets_select', ''),
      '#ajax' => array(
        'callback' => 'formio_presets_form_ajax_callback',
        'wrapper' => 'formio-form-display',
        'method' => 'replace',
      ),
    );

    formio_actions_form($form, $form_state);
    formio_node_form($form, $form_state);
    formio_node_type_field_form($form, $form_state);

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
function formio_presets_form_ajax_callback($form, &$form_state) {
 	$commands = array();
  // The form to target via the api.
  $formio = $form_state['values']['formio_presets_select'];
  // Render the form.
  $block = module_invoke('formio', 'block_view', 'formio_presets', $formio);
  // Render the html in the div we created.
	$commands[] = ajax_command_html('#formio-form-display', $block['content']);

	// The command is a javascript function and the others are arguments
	$commands[] = array(
    'command' => 'doSomething',
    'form' => $form_state['values']['formio_presets_select'],
    'api' => formio_get_project(),
  );

	// Return commands to the system to be executed.
	return array('#type' => 'ajax', '#commands' => $commands);
}

function formio_presets_form_submit($form) {
  if ($form) {

  }
}

/**
 * The actions form.
 *
 * @param array $form
 *   Form that is being added to.
 */
function formio_actions_form(&$form, &$form_state) {
  $form['formio']['formio_presets_actions'] = array(
    '#title' => t('Actions'),
    '#type' => 'select',
    '#options' => array(
      '_none' => t('Choose'),
      'entity_create' => t('Create Entity'),
    ),
    '#default_value' => variable_get('formio_presets_actions', ''),
    '#states' => array(
      'invisible' => array(
        ':input[name="formio_presets_select"]' => array('value' => '_none'),
      ),
    ),
  );

}

function formio_node_form(&$form, &$form_state) {
  $names = node_type_get_names();

  $form['formio']['node_types'] = array(
    '#title' => t('Entity Type'),
    '#type' => 'select',
    '#options' => $names,
    '#states' => array(
      'visible' => array(
        ':input[name="formio_presets_actions"]' => array('value' => 'entity_create'),
      ),
    ),
  );

}

function formio_node_type_field_form(&$form, &$form_state) {
  $formios = formio_get_forms();
  foreach ($formios as $formio_name => $data) {
    $options[$data['path']] = $data['title'];
  }
  $form['formio']['map_fields'] = array(
    '#title' => t('Map Fields'),
    '#type' => 'fieldset',
    '#states' => array(
      'visible' => array(
        ':input[name="formio_presets_actions"]' => array('value' => 'entity_create'),
      ),
    ),
  );
  $fields = field_info_instances('node', 'page');
  foreach ($fields as $name => $info) {
    $form['formio']['map_fields']['field_' . $info['bundle'] . '_' . $info['entity_type'] . '_' . $info['field_name']] = array(
      '#title' => t('Fields'),
      '#type' => 'select',
      '#options' => $options,
    );
  }
}
