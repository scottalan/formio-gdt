<?php

/**
 * Defines a ctools Export UI plugin for Form IO presets.
 */

$plugin = array(
  // As defined in hook_schema().
  'schema' => 'formio_preset',
  // Define a permission users must have to access these pages.
  'access' => 'administer formio',

  // Define the menu item.
  'menu' => array(
    // The 'menu prefix' and 'menu item' create the base path. The default path
    // is 'admin/structure/'. This will give us 'admin/structure/formio/preset',
    // 'admin/structure/formio/preset/add', etc.
    // @see ctools_export_ui_plugin_base_path().
    'menu item' => 'formio',
    'menu title' => 'Form.io',
    'menu description' => 'Form.io preset management.',
    'items' => array(),
  ),

  // Define user interface texts.
  'title singular' => t('Form.io preset'),
  'title plural' => t('Form.io presets'),
  'title singular proper' => t('Form.io Preset'),
  'title plural proper' => t('Form.io Presets'),

  'handler' => array(
    'class' => 'FormioPresetUI',
    'parent' => 'ctools_export_ui',
  ),

  'strings' => array(
    'message' => array(
      'formio' => t('This is a message from Form.io!'),
    ),
  ),

  'use wizard' => TRUE,
  'form info' => array(
    'id' => 'formio_preset',
    'add order' => array(
      'admin' => t('Give your preset a name'),
      'choose' => t('Choose a form'),
      'action' => t('Select an action'),
      'configure' => t('Configure'),
    ),
    'order' => array(
      'admin' => t('Give your preset a name'),
      'choose' => t('Choose a form'),
      'action' => t('Select an action'),
      'configure' => t('Configure'),
      ),
    'forms' => array(
      'choose' => array(
        'form id' => 'formio_preset_choose_form',
      ),
      'action' => array(
        'form id' => 'formio_preset_action_form',
      ),
      'configure' => array(
        'form id' => 'formio_preset_configure_form',
      ),
    ),
  ),

  // Set up an AJAX callback for the style
  'ajax' => array(
//    'custom' => 'panels_stylizer_pane_add_style',
    'custom' => 'formio_preset_form_display',
  ),

  'allowed operations' => array(),
);
