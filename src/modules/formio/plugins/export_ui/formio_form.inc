<?php

/**
 * Defines a ctools Export UI plugin for Form IO presets.
 */

$plugin = array(
  // As defined in hook_schema().
  'schema' => 'formio_form',
  // Define a permission users must have to access these pages.
  'access' => 'administer formio',

  // Define the menu item.
  'menu' => array(
    // The 'menu prefix' and 'menu item' create the base path. The default path
    // is 'admin/structure/'. This will give us 'admin/structure/formio',
    // 'admin/structure/formio/add', etc.
    // @see ctools_export_ui_plugin_base_path().
    'menu item' => 'formio',
    'menu title' => 'Form.io Forms',
    'menu description' => 'Manage imported forms and where to store them.',
    'items' => array(),
  ),

  // Define user interface texts.
  'title singular' => t('Form.io form'),
  'title plural' => t('Form.io forms'),
  'title singular proper' => t('Form.io Form'),
  'title plural proper' => t('Form.io Forms'),

  'form' => array(
    'settings' => 'formio_form_edit_form',
    'validate' => 'formio_form_edit_form_validate',
    'submit' => 'formio_form_edit_form_submit'
  ),

  'add_form' => t('!message', array('!message' => l('Add a new form!', 'admin/structure/formio/add'))),
  'need_key' => t('Looks like you need an API Key. Enter one !here or visit !formio to create one now.', array('!here' => l('Here', 'admin/config/formio/settings'), '!formio' => l('Form.io', 'https://form.io'))),

  'strings' => array(
    'message' => array(

    ),
  ),
);

$plugin['strings']['message']['no items'] = (!empty(formio_apikey())) ? $plugin['add_form'] : $plugin['need_key'];

/**
 * Renders the form when creating a preset.
 *
 * @param array $form
 * @param array $form_state
 * @return array
 */
function formio_form_display_form($form, &$form_state) {

  return $form;
}

/**
 * Implements formio_form_edit_form().
 */
function formio_form_edit_form(&$form, &$form_state) {
  $item = &$form_state['item'];

  // Add formio libraries.
  drupal_add_library('formio', 'seamless');
  // Add the form selection part of the form.
  $has_forms = formio_form_selection_form($form, $form_state, $item->machine_name, $item->name);

  if ($has_forms) {

    $actions = array(
      'drupal' => 'Save to Drupal',
      'formio' => 'Save to Form.io'
    );

    $form['formio_form']['action'] = array(
      '#title' => t('Action'),
      '#type' => 'select',
      '#options' => $actions,
      '#default_value' => isset($item->action) ? $item->action : 'drupal',
    );

    // Make sure that whatever happens, the buttons go to the bottom.
    $form['buttons']['#weight'] = 100;

    // Add buttons.
    $form['buttons']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
    );

    $form['buttons']['delete'] = array(
      '#type' => 'submit',
      '#value' => $item->export_type & EXPORT_IN_CODE ? t('Revert') : t('Delete'),
      '#access' => $form_state['op'] === 'edit' && $item->export_type & EXPORT_IN_DATABASE,
      '#submit' => array('ctools_export_ui_edit_item_form_delete'),
    );
  }
}

/**
 * Provides a selector to choose a Formio form.
 *
 * @param array $form
 *   The current form.
 * @param array $form_state
 *   The current state of the form.
 * @param string $machine_name
 *   The machine_name of an existing form.
 * @param string $name
 *   The name of an existing form.
 *
 * @return bool
 *   Returns FALSE if there are no forms.
 */
function formio_form_selection_form(&$form, &$form_state, $machine_name = NULL, $name = NULL) {
  // Need to include this file.
  ctools_form_include($form_state, 'formio_form', 'formio', 'plugins/export_ui');

  $formio_forms = formio_get_formio_forms();

  if (!empty($formio_forms)) {

    $item = &$form_state['item'];

    $options = array();
    $options['_none'] = t('Choose your form...');
    foreach ($formio_forms as $formio_form) {
      $options[$formio_form['_id']] = $formio_form['name'];
    }

    // Require a name.
    $form['info']['name']['#required'] = TRUE;

    if ($form_state['op'] === 'edit') {
      $form['info']['machine_name']['#disabled'] = TRUE;
      $form['info']['machine_name']['#value'] = $machine_name;
    }

    $html = '<button type="button" id="reload-preset" class="btn btn-info btn-sm">reload</button>';
    $form['formio_form'] = array(
      '#type' => 'fieldset',
      '#title' => t('Preset'),
    );

    $disabled = FALSE;
    if (isset($item->components)) {
      // Check for component fields that may already contain data.
      foreach ($item->components as $component) {
        if ($component['type'] === 'button') {
          continue;
        }
        // If any of the fields contain data disable the selector so the field
        // can't be changed.
        $field_name = 'formio_' . $component['key'];
        $info = field_info_field($field_name);
        if (!empty($info) && field_has_data($field_name)) {
          $disabled = TRUE;
          break;
        }
      }
    }

    $id = drupal_html_id('formio-preset-form');
    $form['formio_form']['id'] = array(
      '#title' => t('Form'),
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => isset($item->formio_id) ? $item->formio_id : '',
      '#prefix' => '<div id="' . $id . '">',
      '#suffix' => '</div>',
      '#disabled' => $disabled,
      '#ajax' => array(
        'callback' => 'formio_preview_ajax_callback',
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

    formio_preview_ajax_form($form);

    return TRUE;
  }

  return FALSE;
}

/**
 * Implements formio_form_edit_form().
 *
 * @todo: Prevent duplicate forms
 */
function formio_form_edit_form_validate(&$form, &$form_state) {

}

/**
 * Implements formio_form_edit_form().
 */
function formio_form_edit_form_submit(&$form, &$form_state) {
  $item = &$form_state['item'];
  // Values that are returned from the export form.
  $values = array('name', 'machine_name', 'action');
  foreach ($values as $key) {
    if(isset($form_state['values'][$key])) {
      $item->{$key} = $form_state['values'][$key];
    }
  }
  $item->action_settings = array();
  $item->formio_id = $form_state['values']['id'];
  // Get form and fill in additional form details.
  $formio_form = formio_get_form($item->formio_id);
  $item->formio_name = $formio_form['name'];
  $item->formio_title = $formio_form['title'];
  $item->api_path = $formio_form['path'];
  $item->formio_type = $formio_form['type'];
  $item->components = $formio_form['components'];
  $item->created = (new DateTime($formio_form['created']))->getTimestamp();
  $item->changed = (new DateTime($formio_form['modified']))->getTimestamp();

  // Build the data field component.
  $data = array(
    'label' => 'Data',
    'key' => 'data',
    'type' => 'serial'
  );
  // Add a data field to store raw json.
  formio_add_field($item->machine_name, $data);

  // Retrieve all components from the form.
  $components = formio_flatten_components($formio_form['components']);
  foreach($components as $component) {
    // Add a new field for each of the components.
    formio_add_field($item->machine_name, $component);
  }

  // Clear ctools object-cache.
  formio_clear_form_cache($item->machine_name);
}

/**
 * Create a new field and field instance for each form component.
 *
 * @param string $bundle
 *   The entity bundle.
 * @param array $component
 *   The Formio form component.
 *
 * @return array
 *   A field instance.
 */
function formio_add_field($bundle, $component) {
  // The name of our entity.
  $entity = 'formio';

  // Convert from camelCase to underscore_notation and prefix with formio_.
  $key = formio_formio_to_drupal($component['key']);
  // The type of component.
  $type = $component['type'];
  $label = $component['label'];

  // Ignore buttons.
  if ($type == 'button') {
    return;
  }

  // See if we have defined a field or field_instance for this component.
  $field = field_info_field($key);
  $instance = field_info_instance($entity, $key, $bundle);

  $field_type = '';
  $widget_type = '';
  $settings = array();

  switch ($type) {
    case 'textfield':
    case 'phoneNumber':
    case 'email':
    case 'password':
    case 'datetime':
    case 'select':
    case 'selectboxes':
      $field_type = 'text';
      $widget_type = 'text_textfield';
      $settings = array();
      break;
    case 'textarea':
    case 'serial':
    case 'address':
    case 'file':
    case 'survey':
    case 'datagrid':
      $field_type = 'text_long';
      $widget_type = 'text_textarea';
      $settings = array('rows' => 5);
      break;
    case 'number':
    case 'currency':
    case 'checkbox':
    case 'radio':
      $field_type = 'number_decimal';
      $widget_type = 'number';
      $settings = array (
        'min' => !empty($component['validate']['min']) ? $component['validate']['min'] : '',
        'max' => !empty($component['validate']['max']) ? $component['validate']['max'] : '',
        'prefix' => !empty($component['prefix']) ? $component['prefix'] : '',
        'suffix' => !empty($component['suffix']) ? $component['suffix'] : '',
      );
      break;
  }

  // Create the field and instance.
  if (empty($field)) {
    $field = array(
      'field_name' => $key,
      'type' => $field_type,
      'entity_types' => array($entity),
    );
    field_create_field($field);
  }
  else {
    if (!field_has_data($field)) {
      field_update_field($field);
    }
  }
  if (empty($instance)) {
    $instance = array(
      'label' => $label,
      'widget' => array('type' => $widget_type),
      'settings' => $settings,
      'display' => array(
        'default' => array(
          'label' => 'hidden',
          'type' => 'text_default',
        ),
        'teaser' => array(
          'label' => 'hidden',
          'type' => 'text_default',
        ),
      ),
      'required' => (isset($component['validate']) && $component['validate']['required']) ? TRUE : FALSE,
      'default_value' => (isset($component['defaultValue'])) ? $component['defaultValue'] : NULL,
      'field_name' => $key,
      'entity_type' => $entity,
      'bundle' => $bundle,
    );
    $instance = field_create_instance($instance);
  }
  else {
    // @todo: Can we update an instance?.
    if (!field_has_data($field)) {
      field_update_instance($instance);
    }
  }
  return $instance;
}

/**
 * Checks to see if a form with the same 'machine_name' already exists.
 *
 * @see menu_edit_menu()
 * @see form_validate_machine_nxame()
 */
function formio_form_name_exists($name, $element, &$form_state) {
  $name_exists = db_query_range('SELECT 1 FROM {formio_form} WHERE name = :name', 0, 1, array(':name' => $name))->fetchField();
  return $name_exists;
}

/**
 * Get a specific form.
 *
 * @param string $id
 *   The unique string that represents a Formio form.
 * @param bool $reset
 *   Choose not to use static cache.
 *
 * @return object
 *   The form object.
 */
function formio_get_form($id, $reset = FALSE) {
  static $drupal_static_fast;
  if (!isset($drupal_static_fast)) {
    $drupal_static_fast = &drupal_static(__FUNCTION__);
  }
  $forms = &$drupal_static_fast;
  if (!$forms) {
    $forms = array();
  }

  if ($reset || !isset($forms[$id])) {
    $formio = formio_rest('form/' . $id);
  }

  return $formio->result;
}
