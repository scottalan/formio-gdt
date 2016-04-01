<?php

$plugin = array(
  'handler' => 'FormioActionUser',
  'entity path' => 'user/%user',
  'hooks' => array(
    'menu' => TRUE,
    'admin_paths' => TRUE,
  ),
);
