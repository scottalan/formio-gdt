<?php
/**
 * @file
 * Action plugin to create an entity on form submission.
 */

$plugin = array(
  'title' => t('Create Entity'),

  'handler' => array(
    'class' => 'FormioEntity',
    'parent' => 'FormioDefault',
  ),

  'has menu' => FALSE,
  'has actions' => TRUE,
  'action ui' => 'formio:entity-ui',
  'access' => TRUE,

);
