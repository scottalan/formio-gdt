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
      'info' => t('!form_title', array('!form_title' => 'Formio: ' . $export->title)),
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
    formio_display_config_form($form, $delta);
  }
  return $form;
}

/**
 * Implements hook_block_save().
 */
function formio_block_save($delta = '', $edit = array()) {
  if (formio_block($delta)) {
    variable_set($delta . ':export', $edit['formio_export']);
    variable_set($delta . ':use_bootswatch', $edit['formio_use_bootswatch']);
    variable_set($delta . ':bootswatch_theme', $edit['formio_bootswatch_theme']);
  }
}

/**
 * Implements hook_block_view().
 */
function formio_block_view($delta = '', $args = array()) {
  if (formio_block($delta)) {
    ctools_include('content');
    $block = array();
    // Params needed for ctools. Listing these out so we know what the args
    // are.
    $type = 'formio_form';
    $subtype = 'formio_form';
    $conf = NULL;
    $keywords = $contexts = array();

    $args['formio_export'] = variable_get($delta . ':export');
    $args['formio_use_bootswatch'] = variable_get($delta . ':use_bootswatch');
    $args['formio_bootswatch_theme'] = variable_get($delta . ':bootswatch_theme');

    // Add additional $args that need to be passed to the renderer.
    $content = ctools_content_render($type, $subtype, $conf, $keywords, $args, $contexts);
    $block['subject'] = '';
    $block['content'] = $content->content;

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
