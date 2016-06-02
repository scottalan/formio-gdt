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
      'info' => t('!form_title', array('!form_title' => 'Formio: ' . $export->name)),
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
    variable_set($delta . ':name', $edit['name']);
    variable_set($delta . ':enable', $edit['enable']);
    variable_set($delta . ':theme', $edit['theme']);
  }
}

/**
 * Implements hook_block_view().
 */
function formio_block_view($delta = '', $content) {
  if (formio_block($delta)) {
    $block = array();
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
