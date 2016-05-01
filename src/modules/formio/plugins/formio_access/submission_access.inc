<?php
/**
 * @file
 * Plugin to validate access for submitting a form to Form.io.
 */

/**
 * Form.io access plugin.
 *
 * Defines access criteria for submitting a form.
 */
$plugin = array(
  'title' => t('Form.io Submission Access'),
  'description' => t('Validate access to submit forms to Form.io'),
  'callback' => 'formio_submit_access_check',
  'settings form' => 'formio_submit_access_check_settings',
  'summary' => 'formio_submit_access_check_summary',
  'default' => array('negate' => 0),
);

/**
 * Settings form.
 */
function formio_submit_access_check_settings($form, &$form_state, $conf) {
  return $form;
}

/**
 * Actual access validation.
 */
function formio_submit_access_check($conf, $context) {
  if ('user can access') {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/**
 * Provide a summary description based upon the checked terms.
 */
function formio_submit_access_check_summary($conf, $context) {
  return t('Returns TRUE if the user is validated.');
}
