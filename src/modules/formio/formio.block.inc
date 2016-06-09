<?php
/**
 * @file
 * Block configuration for a form.io form.
 */

/**
 * Implements hook_block_info().
 */
function formio_block_info() {
  $blocks = array();
  $exports = formio_get_all_exports();
  foreach ($exports as $key => $export) {
    $blocks['formio:' . $key] = array(
      'info' => t('!form_title', array('!form_title' => 'Formio: ' . $export->drupal_title . '(block)')),
      'cache' => DRUPAL_NO_CACHE,
    );
  }

  return $blocks;
}

/**
 * Implements hook_block_configure().
 */
function formio_block_configure($delta = '') {
  $form = array();
  if (formio_block($delta)) {
    $block = block_load('formio', $delta);
    list(, $machine_name) = explode(':', $delta);
    formio_display_config_form($form, $machine_name);
  }
  return $form;
}

/**
 * Implements hook_block_save().
 */
function formio_block_save($delta = '', $edit = array()) {
  if (formio_block($delta)) {
    variable_set($delta . ':machine_name', $edit['machine_name']);
    variable_set($delta . ':enable', $edit['enable']);
    variable_set($delta . ':theme', $edit['theme']);
  }
}

/**
 * Implements hook_block_view().
 */
function formio_block_view($delta = '') {
  if (formio_block($delta)) {
    list(, $machine_name) = explode(':', $delta);
    ctools_include('export');
    $export = ctools_export_crud_load('formio_form', $machine_name);
    $export->is_new = FALSE;
    $block = formio_render((array) $export);

    $b = block_load('formio', $delta);
    $export->enable = variable_get($delta . ':enable', 0);
    $export->theme = variable_get($delta . ':theme', 'paper');
    $content = array(
      '#theme' => 'formio_form',
      '#form' => (array) $export,
      '#attached' => array(
        'drupal_add_http_header' => formio_add_headers(),
        'library' => array(
          array('formio', 'seamless'),
        ),
        // Add custom JavaScript.
        // @todo: Test this out instead of adding to info file.
//      'js' => array(
//        drupal_get_path('module', 'formio') . '/js/formio.js' => array(NULL, 'module'),
//      ),
      ),
    );
    $block = new stdClass();
    $block->content = $content;
    $block = array();
    $block['content'] = $content;

    return $block;
  }
  return FALSE;
}

function formio_block($delta) {
  list($name) = explode(':', $delta);
  if ($name === 'formio') {
    return TRUE;
  }
  return FALSE;
}
