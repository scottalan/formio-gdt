<?php
/**
 * @file
 */

function formio_admin_menu_hook(&$items) {
  $admin_base = array(
    'access arguments' => array('administer formio'),
    'file' => 'formio.admin.inc',
  );
  $items['admin/config/formio'] = array(
    'title' => 'Form.io',
    'description' => 'Manage form.io presets',
    'weight' => -10,
  ) + $admin_base;

  $items['admin/config/formio/settings'] = array(
    'title' => 'Settings',
    'description' => 'Configure settings for Form IO',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('formio_admin_settings'),
    'weight' => 0,
  ) + $admin_base;
}

/**
 * Settings form used to collect configuration used to communicate with Formio.
 *
 * @return array
 *   System settings form.
 */
function formio_admin_settings($form, &$form_state) {
  $form['formio_settings'] = array(
    '#title' => t('Settings'),
    '#type' => 'fieldset',
  );
  $form['formio_settings']['formio_project_url'] = array(
    '#title' => t('Project URL'),
    '#type' => 'textfield',
    '#description' => t('Enter your Form.io project URL'),
    '#default_value' => variable_get('formio_project_url', ''),
  );
  $form['formio_settings']['formio_project_api_key'] = array(
    '#title' => t('Project API Key'),
    '#type' => 'textfield',
    // @todo: Remove this when we use an actual api key, this is for token.
    '#maxlength' => 10000,
    '#description' => t('Enter your Form.io project API key'),
    '#default_value' => variable_get('formio_project_api_key', ''),
  );

  return system_settings_form($form);
}
