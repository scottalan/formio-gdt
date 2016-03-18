<?php

$plugin = array(
  'title' => t('Form IO Form'),
  'description' => t('Renders a form from Form IO'),
  'single' => TRUE,
  'category' => array(t('Form IO'), -10),
  'edit form' => 'formio_content_type_edit_form',
  'render callback' => 'formio_content_type_render',
  'hook theme' => 'formio_content_type_theme',
);

function formio_content_type_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];

  $formio_forms = formio_get_forms();

  foreach ($formio_forms as $name => $values) {
    $options[$name] = $values['title'];
  }

  $form['formio_display']['formio_select_form'] = array(
    '#title' => t('Select Form'),
    '#type' => 'select',
    '#options' => $options,
    '#description' => t('Choose form to display'),
    '#default_value' => $conf['formio_select_form'],
    '#ajax' => array(
      'callback' => 'formio_display_form_ajax_callback',
      'wrapper' => 'formio_form_display',
    ),
  );

  return $form;
}

function formio_content_type_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['values']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function formio_content_type_render($subtype, $conf, $args, $context) {
  $path = isset($conf['formio_select_form']) ? $conf['formio_select_form'] : $args['path'];
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

function formio_content_type_theme(&$theme, $plugin) {
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
