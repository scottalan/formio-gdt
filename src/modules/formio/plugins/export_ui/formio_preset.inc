<?php

/**
 * Defines a ctools Export UI plugin for Form IO presets.
 */

$plugin = array(
  'schema' => 'formio_preset',  // As defined in hook_schema().
  'access' => 'administer formio',  // Define a permission users must have to access these pages.

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
  'title singular' => t('form.io preset'),
  'title plural' => t('form.io presets'),
  'title singular proper' => t('Form.io Preset'),
  'title plural proper' => t('Form.io Presets'),

  'ajax_store' => array(),

  'handler' => array(
    'class' => 'formioPresetUI',
    'parent' => 'ctools_export_ui',
  ),

  'allowed operations' => array(),
);
