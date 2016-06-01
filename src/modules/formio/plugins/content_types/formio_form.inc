<?php
/**
 * @file
 * A ctools content type plugin that renders a form.io form.
 */

use Symfony\Component\Config\Definition\Exception\Exception;

$plugin = array(
  'title' => t('Form.io'),
  'description' => t('Renders an embedded form from Form.io'),
  'single' => TRUE,
  'category' => array(t('form.io'), -10),
  'edit form' => 'formio_formio_form_content_type_edit_form',
  'render callback' => 'formio_formio_form_content_type_render',
  'hook theme' => 'formio_formio_form_content_type_theme',
  'bootswatch api' => 'formio_form_content_type_bootswatch',
  'admin info' => 'formio_formio_form_content_type_admin_info',
);

/**
 * Implements the plugin edit form.
 */
function formio_formio_form_content_type_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];
  formio_display_config_form($form, $conf);
  return $form;
}

/**
 * Implements the plugin edit form submission.
 */
function formio_formio_form_content_type_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['values']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

/**
 * Implements the plugin render callback.
 */
function formio_formio_form_content_type_render($subtype, $conf, $args, $context) {
  if (isset($args['preset'])) {
    $id = $args['form'];
  }

  if (isset($id)) {
    $form = array(
      'id' => $id,
    );
  }
  else {
    $key = isset($conf['formio_export']) ? $conf['formio_export'] : $args['formio_export'];
    $use_theme = isset($conf['formio_use_bootswatch']) ? $conf['formio_use_bootswatch'] : $args['formio_use_bootswatch'];
    $theme = isset($conf['formio_bootswatch_theme']) ? $conf['formio_bootswatch_theme'] : $args['formio_bootswatch_theme'];

    // Get the export name.
    list($pid, $name) = explode(':', $key);

    // @todo: fix this
    if (!isset($name)) {
      $name = $args['formio_export'];
    }
    // @todo: Update {panels_pane} references if an export is updated.
    $export = formio_get_export($name);

    if (!isset($export)) {
      return array();
    }

    $form = array(
      'pid' => $export->pid,
      'id' => $export->id,
      'title' => $export->title,
      'form_name' => $export->form_name,
      'name' => $export->name,
      'action' => $export->action,
      'path' => $export->path,
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
  $apiKey = formio_apikey();
  if (empty($apiKey)) {
    throw new Exception('You must have an api key to continue.');
  }
  return array(
    array('x-token', $apiKey),
    array('Accept', 'application/json'),
    array('Access-Control-Request-Method', 'POST, GET, OPTIONS'),
  );
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
function formio_formio_form_content_type_theme(&$theme, $plugin) {
  $path = $plugin['path'] . '/theme';
  $theme['formio_form'] = array(
    'variables' => array(
      'form' => NULL,
    ),
    'path' => $path,
    'template' => 'formio-form',
  );
}

function formio_formio_form_content_type_admin_info() {
  return t('Renders a form.io form and executes an action defined by the preset.');
}

function formio_formio_form_content_type_admin_title() {
  return t('Form.io Export');
}
