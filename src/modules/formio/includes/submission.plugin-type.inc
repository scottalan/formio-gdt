<?php
/**
 * @file
 */

function formio_submission_plugin_type(&$items) {
  $items['submission_handlers'] = array(
    'label' => 'Submission',
    'use hooks' => FALSE,
    'process' => array(
      'function' => 'formio_submission_process',
      'file' => 'submission.inc',
      'path' => drupal_get_path('module', 'formio') . '/includes',
    ),
    'classes' => array('handler'),
  );
}
