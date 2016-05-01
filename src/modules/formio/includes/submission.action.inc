<?php
/**
 * @file
 */

/**
 * Implements hook_action_info().
 *
 * @see formio_actions_actions().
 */
function formio_submission_action(&$items) {
  $items['formio_submit_action'] = array(
    'type' => 'formio',
    'label' => t('Submit form to form.io'),
    'configurable' => FALSE,
    'triggers' => array('formio_form_submission'),
  );
}

/**
 * Page callback used by the Custom Action URL. This is called when a for has
 * set the action url to "/form.io" .
 *
 * @todo: a wip, need to map data to entity fields and authenticate with token.
 */
function formio_submission_callback() {

  ctools_include('export');
  $stuff = ctools_export_get_schemas();

  $data = file_get_contents('php://input');

  // This is used to validate the request. This will be posted back to form.io
  // and the result will need to be a 200. Add a ?dryrun=1 query to the post.
  $token = getallheaders()['X-Jwt-Token'];

  // Get all enqueued actions.
  $aids = trigger_get_assigned_actions(__FUNCTION__);

  $actions_map = actions_actions_map($aids);

  $actions = actions_list();

  ctools_include('export');
  // TODO: Probably shouldn't be loading all the presets.
  $exports = ctools_export_crud_load('formio_preset', 'new');

  // TODO: Do we need to iterate over each export or can we target just the
  // export we need.
  foreach ($exports as $export) {
    $action = $export->action;
    if (isset($actions_map[$action]) && !empty($actions_map[$action])) {

      $callback = $actions_map[$action]['callback'];
      $request = formio_rest('form/' . $export->id);
      $results = $request->fetchKeyed('path');

      // We should always have a path but in case we don't throw an error.
      $path = $results->forms['path'];
      if (!isset($path)) {
        // TODO: For now try a 404.
        drupal_deliver_page(MENU_NOT_FOUND, 'drupal_deliver_html_page');
      }
      $uri = $results->project . '/'. $path . '/submission';

      // Create the context argument.
      $context = array(
        'id' => $export->id,
        'token' => $token,
        'request' => $request,
        'uri' => $uri,
        'data' => $data,
        'hook' => __FUNCTION__,
        'actions' => $actions,
        'callback' => $callback,
        'args' => func_get_args(),
      );

      if (!empty($aids)) {
        $action_result = actions_do(array_keys($aids), $request, $context);
      }
    }
  }
  drupal_exit();
}

/**
 * An implementation of a Drupal action callback triggered when posting a form.
 *
 * @param object $formio
 *   The formio form.
 * @param array $context
 *   Anything needed to perform the action. The context is build in the trigger.
 */
function formio_submit_action($formio, $context) {

  // Consider @see drupal_deliver_html_page().
  $result = formio_curl_post($context['token'], $context['uri'], $context['data']);

  // TODO: Will this work for multiple actions with the drupal_exit() being called from the trigger?
  if (empty($result['error'])) {
    drupal_json_output(json_encode($result['response']));
  }
}

function formio_submit_action_form($context) {
  return array();
}
