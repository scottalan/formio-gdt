<?php

define('BOOTSWATCH_API', '3');

$plugin = array(
  'title' => t('Form.io Form'),
  'description' => t('Renders an embedded form from Form.io'),
  'single' => TRUE,
  'category' => array(t('form.io'), -10),
  'edit form' => 'formio_form_content_type_edit_form',
  'render callback' => 'formio_form_content_type_render',
  'hook theme' => 'formio_form_content_type_theme',
  'bootswatch api' => 'formio_form_content_type_bootswatch',
);

function formio_form_content_type_edit_form($form, &$form_state) {
  $options = array();

  $conf = $form_state['conf'];

  ctools_include('export');
  $exports = ctools_export_crud_load_all('formio_preset');
  foreach ($exports as $export) {
    $options[$export->pid] = $export->title;
  }
  $form['formio'] = array(
    '#title' => 'Form.io Embed',
    '#type' => 'fieldset',
  );

  $form['formio']['formio_embed'] = array(
    '#type' => 'select',
    '#title' => t('Form'),
    '#options' => $options,
    '#default_value' => $conf['formio_embed'],
  );

  $form['formio']['use_bootswatch'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use bootswatch'),
    '#default_value' => $conf['use_bootswatch'],
  );

  $form['formio']['bootswatch_theme'] = array(
    '#title' => t('Bootswatch Theme'),
    '#default_value' => $conf['bootswatch_theme'],
    '#states' => array(
      'invisible' => array(
        ':input[name="use_bootswatch"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['export_pid'] = array(
    '#type' => 'hidden',
    '#value' => 'formio_embed',
  );

  $swatches = formio_form_content_type_bootswatch();
  if ($swatches == FALSE) {
    $form['formio']['bootswatch_theme']['#type'] = 'textfield';
  }
  else {
    $form['formio']['bootswatch_theme']['#type'] = 'select';
    $form['formio']['bootswatch_theme']['#options'] = $swatches;
  }

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
  // Values could come from a pane or a block.
  // @todo: Maybe the block could just use conf as well.
  $pid = isset($conf[$conf['export_pid']]) ? $conf[$conf['export_pid']] : $args['formio_embed'];
  $use_theme = isset($conf['use_bootswatch']) ? $conf['use_bootswatch'] : $args['use_bootswatch'];
  $theme = isset($conf['bootswatch_theme']) ? $conf['bootswatch_theme'] : $args['bootswatch_theme'];

  $preset = formio_get_preset_by_pid($pid);

  if (isset($pid)) {
    $content = array(
      '#theme' => 'formio_form',
      '#form' => array(
        'pid' => $preset['pid'],
        'id' => $preset['id'],
        'name' => $preset['name'],
        'action' => $preset['action'],
        'theme' => $theme,
        'use_theme' => $use_theme,
      ),
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
  return FALSE;
}

/**
 * Attaching headers to the render array.
 *
 * @return array
 *   An array of headers.
 */
function formio_form_content_type_add_headers() {
  return array(
    array('x-jwt-token', formio_form_content_type_get_token()),
    array('Accept', 'application/json'),
    array('Access-Control-Request-Method', 'POST, GET, OPTIONS'),
  );
}

/**
 * Gets the token needed for the header used for authorization.
 *
 * @return string
 *   The x-jwt-token we need to add to the header for authentication.
 */
function formio_form_content_type_get_token() {
  // If this doesn't work go and get a new one. (May Expire).
  $token = trim(variable_get('formio_project_api_key', FALSE));
  if (!isset($token) || empty($token)) {
    drupal_set_message(t('You need a token file!'), 'error');
  }
  return $token;
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

/**
 * Get bootswatch theme options from their api.
 *
 * @return array|bool
 *   Array of theme names if the api is up.
 */
function formio_form_content_type_bootswatch() {
  $swatches = array();
  $request = drupal_http_request('https://bootswatch.com/api/' . BOOTSWATCH_API . '.json');
  if ($request->code == 200) {
    $bootswatch = drupal_json_decode($request->data);

    foreach ($bootswatch['themes'] as $theme) {
      $name = $theme['name'];
      $key = strtolower($name);
      $swatches[$key] = $name;
    }
    return $swatches;
  }
  return FALSE;
}
