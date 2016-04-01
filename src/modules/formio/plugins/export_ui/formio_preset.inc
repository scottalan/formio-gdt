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
    'menu item' => 'formio/preset',
    'menu title' => 'Form IO',
    'menu description' => 'Formio presets management.',
    'items' => array(),
  ),

  // Define user interface texts.
  'title singular' => t('formio preset'),
  'title plural' => t('formio presets'),
  'title singular proper' => t('Form IO Preset'),
  'title plural proper' => t('Form IO Presets'),

  'ajax_store' => array(),

  'handler' => array(
    'class' => 'formioPresetUI',
    'parent' => 'ctools_export_ui',
  ),

  'allowed operations' => array(),
);
