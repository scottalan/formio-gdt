<?php

$plugin = array(
  // As this is the base class plugin, it shouldn't declare any actions.
  'has menu' => TRUE,
  'handler' => array(
    'class' => 'FormioActionUI',
  ),
  'action forms' => FALSE,
);
