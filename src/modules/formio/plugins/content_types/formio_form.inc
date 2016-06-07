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
  'category' => array(t('Form.io'), -10),
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
 *
 * @param string $subtype
 *   This will be formio_form.
 * @param array $conf
 *   Will only contain data if this is being rendered via panels.
 * @param array $args
 *   Extra arguments. These can come from panels but are primarily used for
 *   rendering a block.
 * @param object $context
 *   Not really used at this point but could require that a specific context
 *   be present.
 *
 * @return array
 *   A render array.
 */
function formio_formio_form_content_type_render($subtype, $conf, $args, $context, $incoming_content) {

  // This is a new export. We are still in the creation process.
  if (isset($conf['is_new']) && $conf['is_new'] && isset($conf['_id'])) {
    $form = array(
      '_id' => $conf['_id'],
      'action' => 'not_drupal',
    );

  }
  else {
    // We might be passing the export in as $conf. If we are then a components
    // array will exist.
    if (!isset($conf['components'])) {
      // @todo: Update {panels_pane} references if an export is updated.
      $export = formio_get_export($conf['machine_name']);

      if (!isset($export)) {
        return array();
      }

      $form = array(
        'fid' => $export->fid,
        '_id' => $export->_id,
        'title' => $export->title,
        'name' => $export->name,
        'machine_name' => $export->machine_name,
        'path' => $export->path,
        'action' => $export->action,
        'theme' => isset($conf['theme']) ? $conf['theme'] : NULL,
        'enable' => isset($conf['enable']) ? $conf['enable'] : 0,
      );
    }
    else {
      $form = $conf;
      $form['theme'] = NULL;
      $form['enable'] = 0;
    }
  }

  // Build the form render array.
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

  $formRenderer = new stdClass();
  $formRenderer->content = $content;
  return $formRenderer;
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

function formio_form_content_type_setting_data($_id) {
  return array(
    'reload' => TRUE,
    'form' => $_id,
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
