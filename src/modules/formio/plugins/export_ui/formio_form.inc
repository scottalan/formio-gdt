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

  'strings' => array(
    'message' => array(
      'formio' => t('This is a message from Form.io!'),
    ),
  ),
);


/**
 * Renders the form when creating a preset.
 *
 * @param array $form
 * @param array $form_state
 * @return array
 */
function formio_form_display_form($form, &$form_state) {
  // Placeholder for rendered formio form.
  $form['formio_render'] = array(
    '#type' => 'fieldset',
    '#title' => t('Form Preview'),
  );
  $form['formio_render']['formio_display'] = array(
    '#markup' => '<div id="formio-form-render"></div>',
  );

  return $form;
}

function formio_form_edit_form(&$form, &$form_state) {
  drupal_add_library('formio', 'seamless');

  unset($form['info']);

  // Add the form selection part of the form.
  $has_forms = formio_form_selection_form($form, $form_state);

  if ($has_forms) {

    $actions = array(
      'drupal' => 'Save to Drupal',
      'formio' => 'Save to Form.io'
    );

    $form[FORMIO_PRESET]['action'] = array(
      '#title' => t('Action'),
      '#type' => 'select',
      '#options' => $actions,
      '#default_value' => isset($form_state['item']->action) ? $form_state['item']->action : 'drupal',
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
      '#value' => $form_state['item']->export_type & EXPORT_IN_CODE ? t('Revert') : t('Delete'),
      '#access' => $form_state['op'] === 'edit' && $form_state['item']->export_type & EXPORT_IN_DATABASE,
      '#submit' => array('ctools_export_ui_edit_item_form_delete'),
    );
  }
}

function formio_form_selection_form(&$form, &$form_state) {
  // Need to include this file.
  ctools_form_include($orm_state, 'formio_form', 'formio', 'plugins/export_ui');

  $formio_forms = formio_get_formio_forms();

  if (!empty($formio_forms)) {

    $options = array();
    $options['_none'] = t('Choose your form...');
    foreach ($formio_forms as $formio_form) {
      $options[$formio_form['_id']] = $formio_form['title'];
    }

    $form += formio_form_display_form($form, $form_state);

    $html = '<button type="button" id="reload-preset" class="btn btn-info btn-sm">reload</button>';
    $form[FORMIO_PRESET] = array(
      '#type' => 'fieldset',
      '#title' => t('Preset'),
    );
    $id = drupal_html_id('formio-preset-form');
    $form[FORMIO_PRESET]['id'] = array(
      '#title' => t('Form.io Form'),
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => isset($form_state['item']->id) ? $form_state['item']->id : '',
      '#prefix' => '<div id="' . $id . '">',
      '#suffix' => '</div>',
      '#disabled' => isset($form_state['item']->pid),
      '#ajax' => array(
        'callback' => 'formio_form_display_ajax',
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

    return TRUE;
  }

  return FALSE;
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
function formio_form_display_ajax($form, $form_state) {
  $commands = array();
  // The form to target via the api.
  $args = array(
    'form' => $form_state['values']['id'],
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
 * Validate for formio_form_edit_form.
 */
function formio_form_edit_form_validate(&$form, &$form_state) {
  // TODO: Prevent duplicate forms.
}

/**
 * Submit for formio_form_edit_form.
 */
function formio_form_edit_form_submit(&$form, &$form_state) {
  // Transfer data from the form to the $item based upon schema values.
  $schema = ctools_export_get_schema($form_state['plugin']['schema']);
  foreach (array_keys($schema['fields']) as $key) {
    if(isset($form_state['values'][$key])) {
      $form_state['item']->{$key} = $form_state['values'][$key];
    }
  }
  // Get form and fill in additional form details.
  $formio_form = formio_get_form($form_state['item']->id);
  $form_state['item']->name = $formio_form['name'];
  $form_state['item']->title = $formio_form['title'];
  $form_state['item']->path = $formio_form['path'];
  $form_state['item']->type = $formio_form['type'];
  $form_state['item']->components = $formio_form['components'];
  $form_state['item']->created = (new DateTime($formio_form['created']))->getTimestamp();
  $form_state['item']->changed = (new DateTime($formio_form['modified']))->getTimestamp();

  // Add data field to store raw json
  $data = array(
    'label' => 'Data',
    'key' => 'data',
    'type' => 'serial'
  );
  formio_add_field($form_state['item']->name, $data);

  $components = formio_flatten_components($formio_form['components']);
  foreach($components as $component) {
    formio_add_field($form_state['item']->name, $component);
  }

  formio_clear_form_cache($form_state['item']->name);
}

/**
 * Add a field to the form.
 *
 * @param $bundle
 *   The entity bundle.
 * @param $component
 *   The field component.
 *
 * @return
 *   Body field instance.
 */
function formio_add_field($bundle, $component) {
  $entity = 'formio';
  // Convert from camelCase to underscore_notation and prefix with formio_.
  $key = formio_formio_to_drupal($component['key']);
  $type = $component['type'];
  $label = $component['label'];

  $field = field_info_field($key);
  $instance = field_info_instance($entity, $key, $bundle);

  $field_type = '';
  $widget_type = '';
  $settings = array();

  switch ($type) {
    case 'textfield':
    case 'phonenumber':
    case 'email':
    case 'password':
      $field_type = $widget_type = 'text';
      $settings = array();
      break;
    case 'textarea':
    case 'serial':
      $field_type = 'text_long';
      $widget_type = 'text_textarea';
      $settings = array('rows' => 5);
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
  if (empty($instance)) {
    $instance = array(
      'field_name' => $key,
      'entity_type' => $entity,
      'bundle' => $bundle,
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
    );
    $instance = field_create_instance($instance);
  }
  return $instance;
}

/**
 * Get a specific form
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
