<?php
/**
 * @file
 * Block configuration for a form.io form.
 */

function formio_block_is_formio($delta) {
  if (strpos($delta, 'formio_form') !== FALSE) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Implements hook_block_info().
 */
function formio_block_info() {
  return array(
    'formio_form' => array(
      'info' => t('Form.io Form'),
    ),
  );
}

/**
 * Implements hook_block_configure().
 */
function formio_block_configure($delta = '') {
  $form = array();
  if (formio_block_is_formio($delta)) {
    formio_display_config_form($form, $delta);
  }
  return $form;
}

/**
 * Implements hook_block_save().
 */
function formio_block_save($delta = '', $edit = array()) {
  if (formio_block_is_formio($delta)) {
    variable_set('formio_export:' . $delta, $edit['formio_export']);
    variable_set('formio_use_bootswatch:' . $delta, $edit['formio_use_bootswatch']);
    variable_set('formio_bootswatch_theme:' . $delta, $edit['formio_bootswatch_theme']);
  }
}

/**
 * Implements hook_block_view().
 */
function formio_block_view($delta = '', $args) {
  if (formio_block_is_formio($delta)) {
    ctools_include('content');
    $block = array();
    // Params needed for ctools. Listing these out so we know what the args
    // are.
    $type = 'formio_form';
    $subtype = 'formio_form';
    $conf = NULL;
    $keywords = $contexts = array();

    // Add additional $args that need to be passed to the renderer.
    $content = ctools_content_render($type, $subtype, $conf, $keywords, $args, $contexts);
    $block['subject'] = '';
    $block['content'] = $content->content;

    return $block;
  }
  return FALSE;
}
