<?php
/**
 * @file
 */

/**
 * Implements hook_formio_ACTION_CALLBACK_form().
 *
 * After creating an action you can implement this hook to add a configuration
 * form on the preset for an action.
 */
function hook_formio_formio_actions_post_action_form($form, &$form_state) {
  // Form displayed for this action.
  $form['custom'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
  );
  return $form;
}
