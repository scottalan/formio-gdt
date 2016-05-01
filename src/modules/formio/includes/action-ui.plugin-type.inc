<?php
/**
 * @file
 */

function formio_action_ui_plugin_type(&$items) {
  $items['action_ui'] = array(
    'label' => 'Action Forms',
    'use hooks' => FALSE,
    'process' => array(
      'function' => 'formio_action_ui_process',
      'file' => 'action-ui.inc',
      'path' => drupal_get_path('module', 'formio') . '/includes',
    ),
    'classes' => array('handler'),
  );
}
