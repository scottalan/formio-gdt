<?php
/**
 * @file
 */

/**
 * Implements hook_action_info().
 *
 * @see formio_actions_actions().
 */
function formio_actions_post_action_info(&$items) {
  $items['formio_actions_post_action'] = array(
    'type' => 'formio',
    'label' => t('Form.io submission notification'),
    'configurable' => FALSE,
    'triggers' => array('formio_post_trigger'),
  );
}

/**
 * An implementation of a Drupal action callback triggered when posting a form.
 *
 * @param object $formio
 *   The formio form.
 * @param array $context
 *   Anything needed to perform the action. The context is build in the trigger.
 */
function formio_actions_post_action($formio, $context) {

  // Consider @see drupal_deliver_html_page().
  $result = formio_curl_post($context['token'], $context['uri'], $context['data']);

  // TODO: Will this work for multiple actions with the drupal_exit() being called from the trigger?
  if (empty($result['error'])) {
    drupal_json_output(json_encode($result['response']));
  }
}
