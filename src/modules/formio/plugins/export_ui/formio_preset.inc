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
    'menu description' => 'Form.io form management.',
    'items' => array(),
  ),

  // Define user interface texts.
  'title singular' => t('Form.io form'),
  'title plural' => t('Form.io forms'),
  'title singular proper' => t('Form.io Form'),
  'title plural proper' => t('Form.io Forms'),

  'form' => array(
    'settings' => 'formio_preset_edit_form',
    'validate' => 'formio_preset_edit_form_validate',
    'submit' => 'formio_preset_edit_form_submit'
  ),

  'strings' => array(
    'message' => array(
      'formio' => t('This is a message from Form.io!'),
    ),
  ),
);

function formio_preset_edit_form(&$form, &$form_state) {
  // Remove title and name as we will get it from the form.
  unset($form['info']);

  // @todo: sassify this!
  $form['#attached']['css'][drupal_get_path('module', 'formio') . '/css/formio.css'] = array();
  drupal_add_library('formio', 'seamless');

  $formio_forms = formio_get_forms();

  if (!empty($formio_forms)) {

    $options = array();
    $options['_none'] = t('Choose your form...');
    foreach ($formio_forms as $formio_form) {
      $options[$formio_form['_id']] = $formio_form['title'];
    }

    $form += formio_preset_display_form($form, $form_state);

    $html = '<button type="button" id="reload-preset" class="btn btn-info btn-sm">reload</button>';
    $form[FORMIO_PRESET] = array(
      '#type' => 'fieldset',
      '#title' => t('Preset Settings !button', array('!button' => $html)),
    );
    $id = drupal_html_id('formio-preset-form');
    $form[FORMIO_PRESET]['id'] = array(
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

/**
 * Validate for formio_preset_edit_form.
 */
function formio_preset_edit_form_validate(&$form, &$form_state) {
  // TODO: Prevent duplicate forms.
}

/**
 * Submit for formio_preset_edit_form.
 */
function formio_preset_edit_form_submit(&$form, &$form_state) {
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

  $components = formio_flatten_components($formio_form['components']);
  foreach($components as $component) {
    formio_add_field($formio_form['name'], $component);
  }
}

/**
 * Add a field to the form
 */
/**
 * Adds default body field to a node type.
 *
 * @param $type
 *   A node type object.
 * @param $label
 *   The label for the body instance.
 *
 * @return
 *   Body field instance.
 */
function formio_add_field($bundle, $component) {
  $entity = 'formio';
  $key = $component['key'];
  $type = $component['type'];
  $label = $component['label'];

  $field = field_info_field($key);
  $instance = field_info_instance($entity, $key, $bundle);

  switch ($type) {
    case 'textfield':
    case 'phonenumber':
    case 'email':
    case 'password':
      if (empty($field)) {
        $field = array(
          'field_name' => $key,
          'type' => 'text',
          'entity_types' => array($entity),
        );
      }
      $field = field_create_field($field);
      if (empty($instance)) {
        $instance = array(
          'field_name' => $key,
          'entity_type' => $entity,
          'bundle' => $bundle,
          'label' => $label,
          'widget' => array('type' => 'text'),
          'settings' => array('display_summary' => TRUE),
          'display' => array(
            'default' => array(
              'label' => 'hidden',
              'type' => 'text_default',
            ),
            'teaser' => array(
              'label' => 'hidden',
              'type' => 'text_summary_or_trimmed',
            ),
          ),
        );
        $instance = field_create_instance($instance);
      }
      break;
  }
  return $instance;
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
    '#title' => t('Form Preview'),
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
    $formio = formio_rest('form', array('type' => 'form', 'limit' => 99999));
  }

  return $formio->result;
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
