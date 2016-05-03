<?php

class FormioEntityUI extends FormioActionUI {

  /**
   * Implements init().
   *
   * Initialization method for the plugin.
   */
  function init($plugin) {
    parent::init($plugin);
  }

  function entity_type_form(&$form, &$form_state) {
    $options = array();
    $entities = entity_get_info();

    foreach ($entities as $entity_type => $info) {
      if (entity_type_is_fieldable($entity_type)) {
        $options[$entity_type] = $info['label'];
      }
    }

    $default = isset($form_state['item']->settings['entity_type']) ? $form_state['item']->settings['entity_type'] : '';
    $form['settings']['entity_type'] = array(
      '#title' => t('Entity Type'),
      '#type' => 'select',
      '#options' => $options,
      '#description' => t('Select the entity type you want to map form fields to.'),
      '#default_value' => $default,
    );
  }

  function entity_type_form_submit(&$form, &$form_state) {
    $form_state['item']->settings['entity_type'] = $form_state['values']['entity_type'];
  }

  function entity_bundle_form(&$form, &$form_state) {
    $options = array();
    $entity_type = $form_state['item']->settings['entity_type'];
    $info = entity_get_info($entity_type);

    foreach ($info['bundles'] as $name => $bundle) {
      $options[$name] = $bundle['label'];
    }

    $default = isset($form_state['item']->settings['entity_bundle']) ? $form_state['item']->settings['entity_bundle'] : '';
    $form['settings']['entity_bundle'] = array(
      '#title' => t('Bundle'),
      '#type' => 'select',
      '#options' => $options,
      '#description' => t('Choose the entity bundle.'),
      '#default_value' => $default,
    );
  }

  function entity_bundle_form_submit(&$form, &$form_state) {
    $form_state['item']->settings['entity_bundle'] = $form_state['values']['entity_bundle'];
  }

  /**
   * @todo Decide whether to use entity_get_property_info() or field_info_instances()??
   */
  function field_map_form(&$form, &$form_state) {
    $options = array();
    $entity_type = $form_state['item']->settings['entity_type'];
    $bundle = $form_state['item']->settings['entity_bundle'];

    ctools_form_include($form_state, 'formio', 'formio');
    $formio = formio_rest('form/' . $form_state['item']->id);

    $components = $formio->fetchComponents();

    $options['_none'] = t('None');
    foreach ($components as $component) {
      $type = t('@type', array('@type' => $component['type']));
      $options[$component['key']] = $component['label'] . ' ' . ' - (' . $type . ')';
    }

    // The title is always required so remove '_none' from the options.
    $toptions = $options;
    unset($toptions['_none']);
    // Add a special field for the title.
    $default = isset($form_state['item']->settings['entity_field_map']['title']) ? $form_state['item']->settings['entity_field_map']['title'] : '';
    $form['settings']['map:title'] = array(
      '#title' => t('Title'),
      '#type' => 'select',
      '#options' => $toptions,
      '#default_value' => $default,
    );

    $fields = field_info_instances($entity_type, $bundle);
    foreach ($fields as $name => $info) {
      $default = isset($form_state['item']->settings['entity_field_map'][$name]) ? $form_state['item']->settings['entity_field_map'][$name] : '';
      $form['settings']['map:' . $info['field_name']] = array(
        '#title' => t('@name', array('@name' => $info['label'])),
        '#description' => t('@description', array('@description' => $info['description'])),
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => $default,
      );
    }
  }

  function field_map_form_submit(&$form, &$form_state) {
    foreach ($form_state['values'] as $name => $value) {
      $parts = explode(':', $name);
      if (isset($parts[0]) && $parts[0] == 'map' && $value != '_none') {
        $form_state['item']->settings['entity_field_map'][$parts[1]] = $value;
      }
    }
  }
}
