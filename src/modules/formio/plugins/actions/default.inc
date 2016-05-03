<?php
/**
 * @file
 * Action plugin to define the default action when a form is submitted.
 */

$plugin = array(
  'title' => t('Form.io Submission'),
  'handler' => array(
    'class' => 'FormioDefault',
  ),
  'has menu' => TRUE,
  'has actions' => TRUE,
  'access' => TRUE,

);
