<?php
/**
 * @file
 * Form.io action for posting to a Webform submission.
 */

/**
 * Implements hook_action_info().
 *
 * @todo document the importance of choosing the name of the 'action'.
 * @todo This is used to find the file and the info function in that file.
 */
function formio_entity_action(&$items) {
  $items['formio_action_entity_creation'] = array(
    'type' => 'formio',
    'label' => t('Create entity on submission'),
    'configurable' => FALSE,
    'triggers' => array('formio_form_submission'),
  );
}

function formio_add_formio_preset_entity_action_edit_form(&$form, &$form_state) {
  $options = array();
  $entities = entity_get_info();

  foreach ($entities as $entity_type => $info) {
    if (entity_type_is_fieldable($entity_type)) {
      $options[$entity_type] = $info['label'];
    }
  }

  $form['settings']['entity_type'] = array(
    '#title' => t('Entity Type'),
    '#type' => 'select',
    '#options' => $options,
    '#description' => t('Select the entity type you would like to use to map form fields.'),
    '#default_value' => isset($form_state['settings']['entity_type']) ? $form_state['settings']['entity_type'] : '',
  );
}

function formio_add_formio_preset_entity_action_edit_form_submit(&$form, &$form_state) {
  $form_state['item']->settings['entity_type'] = $form_state['values']['entity_type'];
}

function formio_add_formio_preset_entity_bundle_edit_form(&$form, &$form_state) {
  $options = array();
  $entity_type = $form_state['item']->settings['entity_type'];
  $info = entity_get_info($entity_type);

  foreach ($info['bundles'] as $name => $bundle) {
    $options[$name] = $bundle['label'];
  }

  $form['settings']['entity_bundle'] = array(
    '#title' => t('Bundle'),
    '#type' => 'select',
    '#options' => $options,
    '#description' => t('Choose the entity bundle.'),
    '#default_value' => isset($form_state['item']->settings['entity_bundle']) ? $form_state['item']->settings['entity_bundle'] : 'comment',
  );
}

function formio_add_formio_preset_entity_bundle_edit_form_submit(&$form, &$form_state) {
  $form_state['item']->settings['entity_bundle'] = $form_state['values']['entity_bundle'];
}

/**
 * @todo Decide whether to use entity_get_property_info() or field_info_instances()??
 */
function formio_add_formio_preset_entity_field_map_edit_form(&$form, &$form_state) {
  $options = array();
  $entity_type = $form_state['item']->settings['entity_type'];
  $bundle = $form_state['item']->settings['entity_bundle'];
  $property_info = entity_get_property_info($entity_type);

  $formio = formio_rest('form/' . $form_state['item']->id);

  $components = $formio->fetchComponents();

  $options['_none'] = t('None');
  foreach ($components as $component) {
    $options[$component['key']] = $component['label'];
  }

  $fields = field_info_instances($entity_type, $bundle);
  foreach ($fields as $name => $info) {
    $form['settings']['map:' . $info['field_name']] = array(
      '#title' => t('@name', array('@name' => $info['label'])),
      '#description' => t('@description', array('@description' => $info['description'])),
      '#type' => 'select',
      '#options' => $options,
    );
  }
}

function formio_add_formio_preset_entity_field_map_edit_form_submit(&$form, &$form_state) {
  $form_state['item']->settings['entity_field_map'] = array();
  foreach ($form_state['values'] as $name => $value) {
    list($key, $field) = explode(':', $name);
    if ($key == 'map') {
      $form_state['item']->settings['entity_field_map'][$field] = $value;
    }
  }
}

// Edit forms //

function formio_edit_formio_preset_entity_action_edit_form(&$form, &$form_state) {
  formio_add_formio_preset_entity_action_edit_form($form, $form_state);
}

function formio_edit_formio_preset_entity_action_edit_form_submit(&$form, &$form_state) {
  formio_add_formio_preset_entity_action_edit_form_submit($form, $form_state);
}

function formio_edit_formio_preset_entity_bundle_edit_form(&$form, &$form_state) {
  formio_add_formio_preset_entity_bundle_edit_form($form, $form_state);
}

function formio_edit_formio_preset_entity_bundle_edit_form_submit(&$form, &$form_state) {
  formio_add_formio_preset_entity_bundle_edit_form_submit($form, $form_state);
}

function formio_edit_formio_preset_entity_field_map_edit_form(&$form, &$form_state) {
  formio_add_formio_preset_entity_field_map_edit_form($form, $form_state);
}

function formio_edit_formio_preset_entity_field_map_edit_form_submit(&$form, &$form_state) {
  formio_add_formio_preset_entity_field_map_edit_form_submit($form, $form_state);
}

/**
 * Action callback that starts entity creation.
 */
function formio_action_entity_creation() {

  $entity = new Entity();
}
