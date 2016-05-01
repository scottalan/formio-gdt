<?php
/**
 * @file
 */

function formio_action_plugin_type(&$items) {
  $items['actions'] = array(
    'label' => 'Actions on Submission',
    'use hooks' => FALSE,
    'process' => array(
      'function' => 'formio_actions_process',
      'file' => 'action.inc',
      'path' => drupal_get_path('module', 'formio') . '/includes',
    ),
    'classes' => array('handler'),
  );
}
