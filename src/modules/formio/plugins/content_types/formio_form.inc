<?php
/**
 * @file
 * A ctools content type plugin that renders a form.io form.
 */

use Symfony\Component\Config\Definition\Exception\Exception;

$plugin = array(
  'title' => t('Form.io'),
  'description' => t('Renders an embedded form from Form.io'),
  'category' => array(t('Form.io'), -10),
  'edit form' => 'formio_formio_form_content_type_edit_form',
  'render callback' => 'formio_formio_form_content_type_render',
  'hook theme' => 'formio_formio_form_content_type_theme',
  'bootswatch api' => 'formio_form_content_type_bootswatch',
  'admin info' => 'formio_formio_form_content_type_admin_info',
);

/**
 * Creates a subtype for every export.
 *
 * This will add a new pane for each export that is created. This will allow
 * placing multiple forms via ctools page manager.
 *
 * @return array
 *   Content types that render a form for each export.
 */
function formio_formio_form_content_type_content_types($plugin) {
  $content_types = array();
  ctools_include('export');
  $exports = ctools_export_crud_load_all('formio_form');
  foreach ($exports as $machine_name => $export) {
    $title = t('%title', array('%title' => $export->drupal_title));
    $content_types[$machine_name] = array(
      'title' => $title,
      'description' => t('Renders the ' . $title . ' embedded form.'),
      'icon' => ctools_image_path('favicon.ico', 'formio'),
      'category' => array(
        'Form.io',
        0,
      ),
    );
  }
  return $content_types;
}

/**
 * Implements the plugin edit form.
 *
 * This is the edit form for the content type. It's used to collect additional
 * information apart from the export.
 */
function formio_formio_form_content_type_edit_form(&$form, &$form_state) {
  $conf = $form_state['conf'];
  $subtype = $form_state['subtype_name'];

  ctools_include('export');
  $export = ctools_export_crud_load('formio_form', $subtype);

  // Build the form.
  $form['formio'] = array(
    '#title' => 'Form',
    '#type' => 'fieldset',
  );

  $form['formio']['enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use bootswatch'),
    '#default_value' => is_array($conf) ? $conf['enable'] : variable_get($conf . ':use_bootswatch', ''),
    '#ajax' => array(
      'callback' => 'formio_ajax_update_preview',
//      'wrapper' => 'formio-form-render',
      'path' => 'system/formio_ajax',
//      'method' => 'replace',
    ),
  );

  $form['formio']['theme'] = array(
    '#title' => t('Bootswatch Theme'),
    '#default_value' => is_array($conf) ? $conf['theme'] : variable_get($conf . ':bootswatch_theme', ''),
    '#states' => array(
      'invisible' => array(
        ':input[name="enable"]' => array('checked' => FALSE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'formio_ajax_update_preview',
//      'wrapper' => 'formio-form-render',
      'path' => 'system/formio_ajax',
//      'method' => 'replace',
    ),
    '#attached' => array(
      'library' => array(
        array('formio', 'seamless'),
      ),
//      'js' => array(
//        drupal_get_path('module', 'formio') . '/js/formio.js' => array(NULL, 'module'),
//      ),
    ),
  );

  $form['formio']['action_settings']['redirect'] = array(
    '#type' => 'value',
    '#value' => isset($export->action_settings['redirect']) ? $export->action_settings['redirect'] : '',
  );

  // Get possible Bootswatch themes from the Bootswatch api.
  $swatches = formio_bootswatch();
  // If we have trouble communicating with the api then just present the user
  // with a textfield so they can enter the name of the theme.
  if ($swatches == FALSE) {
    $form['formio']['theme']['#type'] = 'textfield';
  }
  // If we got a successful response from Bootswatch then populate a select
  // list with the options.
  else {
    $form['formio']['theme']['#type'] = 'select';
    $form['formio']['theme']['#options'] = $swatches;
  }

//  ctools_form_include($form_state, 'content');
//  ctools_form_include($form_state, 'formio_form', 'formio', 'plugins/content_types/formio_form');
//  formio_display_config_form($form, $subtype);
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

  if (isset($subtype) && !empty($subtype)) {
    $form = (array) formio_get_export($subtype);
    $diff = array_diff_key($conf, $form);
    $form += $diff;
  }
  // This is a new export. We are still in the creation process.
  else if (isset($conf['is_new']) && $conf['is_new'] && isset($conf['_id'])) {
    $form = formio_get_form($conf['_id']);

  }
  // We might be passing the export in as $conf. If we are then a components
  // array will exist.
  else if (!empty($conf) && !isset($conf['components'])) {
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


  // Build the form render array.
  $content = array(
    '#theme' => 'formio_form',
    '#form' => $form,
    '#attached' => array(
      'drupal_add_http_header' => formio_add_headers(),
      // Add custom JavaScript.
      // @todo: Test this out instead of adding to info file.
//      'js' => array(
//        drupal_get_path('module', 'formio') . '/js/formio.js' => array(NULL, 'module'),
//      ),
    ),
  );

  // Called first on blocks.
//  drupal_add_library('formio', 'seamless');
  $formRenderer = new stdClass();
  $formRenderer->content = $content;
  return $formRenderer;
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
