<?php
/**
 * @file
 * Action plugin to create an entity on form submission.
 */

$plugin = array(
  'title' => t('Create Formio Entity'),

  'handler' => array(
    'class' => 'FormioDrupal',
    'parent' => 'FormioDefault',
  ),

  'has menu' => FALSE,
  'has actions' => TRUE,
  'access' => TRUE,

);
