<?php

define('BOOTSWATCH_API', '3');

$plugin = array(
  'title' => t('Form.io'),
  'description' => t('Renders an embedded form from Form.io'),
  'single' => TRUE,
  'category' => array(t('form.io'), -10),
  'edit form' => 'formio_form_content_type_edit_form',
  'render callback' => 'formio_form_content_type_render',
  'hook theme' => 'formio_form_content_type_theme',
  'bootswatch api' => 'formio_form_content_type_bootswatch',
  'admin info' => t('Renders a form.io form and executes an action defined by the preset.')
);

function formio_form_content_type_edit_form($form, &$form_state) {
  $options = array();

  $conf = $form_state['conf'];

  formio_display_config_form($form, $conf);

  return $form;
}

function formio_form_content_type_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['values']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function formio_form_content_type_render($subtype, $conf, $args, $context) {
  if (isset($args['preset'])) {
    $id = $args['form'];
  }

  if (isset($id)) {
    $form = array(
      'id' => $id,
    );
  }
  else {
    // Values could come from a pane or a block.
    // @todo: Maybe the block could just use conf as well.
    $name = isset($conf['formio_preset_name']) ? $conf['formio_preset_name'] : $args['formio_preset_name'];
    $use_theme = isset($conf['formio_use_bootswatch']) ? $conf['formio_use_bootswatch'] : $args['formio_use_bootswatch'];
    $theme = isset($conf['formio_bootswatch_theme']) ? $conf['formio_bootswatch_theme'] : $args['formio_bootswatch_theme'];

    $preset = ctools_export_crud_load(FORMIO_PRESET, $name);

    $form = array(
      'pid' => $preset->pid,
      'id' => $preset->id,
      'name' => $preset->name,
      'action' => $preset->action,
      'theme' => $theme,
      'use_theme' => $use_theme,
    );

  }

  if (!isset($form)) {
    return FALSE;
  }

  $content = array(
    '#theme' => 'formio_form',
    '#form' => $form,
    '#attached' => array(
      'drupal_add_http_header' => formio_form_content_type_add_headers(),
      'library' => array(
        array('formio', 'seamless'),
      ),
      // Add custom JavaScript.
      'js' => array(
        drupal_get_path('module', 'formio') . '/js/formio.js' => array(NULL, 'module'),
      ),
    ),
  );

  $block = new stdClass();
  $block->title = '';
  $block->content = $content;
  return $block;
}

/**
 * Attaching headers to the render array.
 *
 * @return array
 *   An array of headers.
 */
function formio_form_content_type_add_headers() {
  return array(
    array('x-jwt-token', formio_form_content_type_api_key()),
    array('Accept', 'application/json'),
    array('Access-Control-Request-Method', 'POST, GET, OPTIONS'),
  );
}

/**
 * Gets the api key used in the header for authorization.
 *
 * @return string
 *   The x-token we need to add to the header for authentication.
 */
function formio_form_content_type_api_key() {
  // If this doesn't work go and get a new one. (May Expire).
  $apikey = trim(variable_get('formio_project_api_key', FALSE));
  if (!isset($apikey) || empty($apikey)) {
    drupal_set_message(t('Looks like you need an API Key. Visit !formio and create one.', array('!formio', l('Form.io', 'https://form.io'))), 'warning');
  }
  return $apikey;
}

function formio_form_content_type_setting_data($id) {
  return array(
    'reload' => TRUE,
    'form' => $id,
    'api' => variable_get('formio_project_url', NULL),
    'target' => '#formio-form-render',
  );
}

/**
 * Implements ctools content types plugin hook theme.
 */
function formio_form_content_type_theme(&$theme, $plugin) {
  $path = $plugin['path'] . '/theme';
  $theme['formio_form'] = array(
    'variables' => array(
      'form' => NULL,
    ),
    'path' => $path,
    'template' => 'formio-form',
  );
}
