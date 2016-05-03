<?php
/**
 * @file
 * Action plugin to define the form used when creating an entity on
 * form submission.
 */

/**
 * Defines a plugin to inject forms into a preset for gathering information
 * about creating an
 */
$plugin = array(
  'title' => t('Create Entity'),

  'handler' => array(
    'class' => 'FormioEntityUI',
  ),

  // This is the 'actions' action that goes with this 'action_ui'.
  'action' => 'formio:entity',

  // Forms, or rather 'step' forms that will be used to gather information
  // needed to create an entity on form submission.
  'action forms' => array(
    'entity_type' => t('Entity Type'),
    'entity_bundle' => t('Bundle'),
    'field_map' => t('Map Fields'),
  ),
);
