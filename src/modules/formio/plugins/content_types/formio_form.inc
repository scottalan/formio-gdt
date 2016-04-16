<?php

define('BOOTSWATCH_API', '3');

$plugin = array(
  'title' => t('Form.io Form'),
  'description' => t('Renders a form from Form.io'),
  'single' => TRUE,
  'category' => array(t('form.io'), -10),
  'edit form' => 'formio_form_content_type_edit_form',
  'render callback' => 'formio_form_content_type_render',
  'hook theme' => 'formio_form_content_type_theme',
);

function formio_form_content_type_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];

  $exports = ctools_export_crud_load_all('formio_preset');
  foreach ($exports as $export) {
    $options[$export->_id] = $export->title;
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
    '#type' => 'select',
    '#title' => t('Bootswatch Theme'),
    '#options' => formio_form_content_type_bootswatch(),
    '#default_value' => $conf['bootswatch_theme'],
    '#states' => array(
      'invisible' => array(
        ':input[name="use_bootswatch"]' => array('checked' => FALSE),
      ),
    ),
  );

  return $form;
}

function formio_form_content_type_bootswatch() {
  $swatches = array();
  $request = drupal_http_request('https://bootswatch.com/api/' . BOOTSWATCH_API . '.json');
  if ($request->code != 200) {
    form_set_error(NULL, t('Not able to access BootSwatch at the moment.'));
  }
  $bootswatch = drupal_json_decode($request->data);

  foreach ($bootswatch['themes'] as $theme) {
    $name = $theme['name'];
    $key = strtolower($name);
    $swatches[$key] = $name;
  }

  return $swatches;
}

function formio_form_content_type_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['values']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function formio_form_content_type_render($subtype, $conf, $args, $context) {
  $path = isset($conf['formio_embed']) ? $conf['formio_embed'] : $args['path'];
  if (isset($path)) {
    $content = array(
      '#theme' => 'formio_form_wrapper',
      '#form' => array(
        '#path' => $path,
        '#theme' => 'formio_form',
      ),
    );

    $block = new stdClass();
    $block->title = '';
    $block->content = $content;
    return $block;
  }
  return FALSE;
}

function formio_form_content_type_theme(&$theme, $plugin) {
  $path = $plugin['path'] . '/theme';
  $theme['formio_form_wrapper'] = array(
    'variables' => array(
      'form' => NULL,
    ),
    'path' => $path,
    'template' => 'formio-form-wrapper',
  );
  $theme['formio_form'] = array(
    'render element' => 'element',
    'path' => $path,
    'template' => 'formio-form',
  );
}
