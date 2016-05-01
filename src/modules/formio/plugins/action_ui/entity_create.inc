<?php
/**
 * @file
 * Action plugin to define the form used when creating an entity on
 * form submission.
 */

$plugin = array(
  'title' => t('Create Entity'),

  'handler' => array(
    'class' => 'FormioEntityCreateUI',
  ),

  'action form' => array(
    'entity:action' => array(
      'title' => t('Entity Type'),
      'form' => 'formio_entity_type_action_form',
    ),
    'entity:bundle' => array(
      'title' => t('Bundle'),
      'form' => 'formio_entity_bundle_action_form',
    ),
    'entity:field_map' => array(
      'title' => t('Map Fields'),
      'form' => 'formio_entity_field_map_action_form',
    ),
  ),

  'action' => 'entity',

);

function formio_entity_type_action_form(&$form, &$form_state) {
  $options = array();
  $entities = entity_get_info();

  foreach ($entities as $entity_type => $info) {
    if (entity_type_is_fieldable($entity_type)) {
      $options[$entity_type] = $info['label'];
    }
  }

  $default = $form_state['item']->settings['entity_type'];
  $form['settings']['entity_type'] = array(
    '#title' => t('Entity Type'),
    '#type' => 'select',
    '#options' => $options,
    '#description' => t('Select the entity type you would like to use to map form fields.'),
    '#default_value' => isset($default) ? $default : '',
  );
}

function formio_entity_type_action_form_submit(&$form, &$form_state) {
  $form_state['item']->settings['entity_type'] = $form_state['values']['entity_type'];
}

function formio_entity_bundle_action_form(&$form, &$form_state) {
  $options = array();
  $entity_type = $form_state['item']->settings['entity_type'];
  $info = entity_get_info($entity_type);

  foreach ($info['bundles'] as $name => $bundle) {
    $options[$name] = $bundle['label'];
  }

  $default = $form_state['item']->settings['entity_bundle'];
  $form['settings']['entity_bundle'] = array(
    '#title' => t('Bundle'),
    '#type' => 'select',
    '#options' => $options,
    '#description' => t('Choose the entity bundle.'),
    '#default_value' => isset($default) ? $default : '',
  );
}

function formio_entity_bundle_action_form_submit(&$form, &$form_state) {
  $form_state['item']->settings['entity_bundle'] = $form_state['values']['entity_bundle'];
}

/**
 * @todo Decide whether to use entity_get_property_info() or field_info_instances()??
 */
function formio_entity_field_map_action_form(&$form, &$form_state) {
  $options = array();
  $entity_type = $form_state['item']->settings['entity_type'];
  $bundle = $form_state['item']->settings['entity_bundle'];
  $property_info = entity_get_property_info($entity_type);

  $formio = formio_rest('form/' . $form_state['item']->id);

  $components = $formio->fetchComponents();

  $options['_none'] = t('None');
  foreach ($components as $component) {
    $options[$component['key']] = $component['label'] . ' ' . '(' . $component['type'] . ')';
  }

  $fields = field_info_instances($entity_type, $bundle);
  foreach ($fields as $name => $info) {
    $default = $form_state['item']->settings['entity_field_map'][$name];
    $form['settings']['map:' . $info['field_name']] = array(
      '#title' => t('@name', array('@name' => $info['label'])),
      '#description' => t('@description', array('@description' => $info['description'])),
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => isset($default) ? $default : '',
    );
  }
}

function formio_entity_field_map_action_form_submit(&$form, &$form_state) {
  foreach ($form_state['values'] as $name => $value) {
    $parts = explode(':', $name);
    if (isset($parts[0]) && $parts[0] == 'map') {
      $form_state['item']->settings['entity_field_map'][$parts[1]] = $value;
      unset($key);
    }
  }
}

function formio_entity_action() {

}

function formio_entity_create() {}

