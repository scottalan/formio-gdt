<?php
/**
 * @file
 * Block configuration for a form.io form.
 */

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
  $options = array();

  switch ($delta) {
    case 'formio_form':
      ctools_include('export');
      $exports = ctools_export_crud_load_all('formio_preset');
      foreach ($exports as $export) {
        $options[$export->id] = $export->title;
      }
      $form['formio'] = array(
        '#title' => 'Form.io Embed',
        '#type' => 'fieldset',
      );

      $form['formio']['formio_block_pid'] = array(
        '#type' => 'select',
        '#title' => t('Form'),
        '#options' => $options,
        '#default_value' => variable_get('formio_block_pid', ''),
      );

      $form['formio']['formio_block_bootswatch'] = array(
        '#type' => 'checkbox',
        '#title' => t('Use bootswatch'),
        '#default_value' => variable_get('formio_block_bootswatch', ''),
      );

      $form['formio']['formio_block_theme'] = array(
        '#title' => t('Bootswatch Theme'),
        '#default_value' => variable_get('formio_block_theme', ''),
        '#states' => array(
          'invisible' => array(
            ':input[name="formio_block_bootswatch"]' => array('checked' => FALSE),
          ),
        ),
      );

      // TODO: Figure this out.
      ctools_include('plugins');
      $plugin = ctools_get_plugins('formio', 'content_types', 'formio_form');
      $swatches = formio_form_content_type_bootswatch();
      if ($swatches == FALSE) {
        $form['formio']['formio_block_theme']['#type'] = 'textfield';
      }
      else {
        $form['formio']['formio_block_theme']['#type'] = 'select';
        $form['formio']['formio_block_theme']['#options'] = $swatches;
      }
  }
  return $form;
}

/**
 * Implements hook_block_save().
 */
function formio_block_save($delta = '', $edit = array()) {
  // This example comes from node.module.
  switch ($delta) {
    case 'formio_form':
      variable_set('formio_block_pid', $edit['formio_block_pid']);
      variable_set('formio_block_bootswatch', $edit['formio_block_bootswatch']);
      variable_set('formio_block_theme', $edit['formio_block_theme']);
  }
}

/**
 * Implements hook_block_view().
 */
function formio_block_view($delta = '', $args) {
  ctools_include('content');
  $block = array();
  switch ($delta) {
    case 'formio_form':

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
      break;
  }

  return $block;
}
