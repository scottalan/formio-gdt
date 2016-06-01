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
  $forms = formio_get_formio_forms();
  foreach ($forms as $form) {
    $formio_forms = db_select(FORMIO_PRESET, 'ff')
      ->fields('ff')
      ->execute()
      ->fetchAllAssoc('form_name');

    foreach ($formio_forms as $key => $form) {
      $blocks['formio_block_' . $key] = array(
        'info' => t('!form_title', array('!form_title' => 'Formio: ' . $form->title)),
      );
    }
    break;

  }

  return $blocks;
}

/**
 * Implements hook_block_configure().
 */
function formio_block_configure($delta = '') {
  $form = array();
  if (strpos($delta, 'formio_block_') !== FALSE) {
    formio_display_config_form($form, $delta);
  }
  return $form;
}

/**
 * Implements hook_block_save().
 */
function formio_block_save($delta = '', $edit = array()) {
  if (strpos($delta, 'formio_block_') !== FALSE) {
    variable_set('formio_export:' . $delta, $edit['formio_export']);
    variable_set('formio_use_bootswatch:' . $delta, $edit['formio_use_bootswatch']);
    variable_set('formio_bootswatch_theme:' . $delta, $edit['formio_bootswatch_theme']);
  }
}

/**
 * Implements hook_block_view().
 */
function formio_block_view($delta = '', $args = array()) {
  if (strpos($delta, 'formio_block_') !== FALSE) {
    ctools_include('content');
    $block = array();
    // Params needed for ctools. Listing these out so we know what the args
    // are.
    $type = 'formio_form';
    $subtype = 'formio_form';
    $conf = NULL;
    $keywords = $contexts = array();

    $args['formio_export'] = substr($delta, 12, 0);

    // Add additional $args that need to be passed to the renderer.
    $content = ctools_content_render($type, $subtype, $conf, $keywords, $args, $contexts);
    $block['subject'] = '';
    $block['content'] = $content->content;

    return $block;
  }
  return FALSE;
}
