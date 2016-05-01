<?php
/**
 * @file
 * Action plugin to define the default action when a form is submitted.
 */

$plugin = array(
  'title' => t('Form.io Submission'),
  'handler' => array(
    'class' => 'FormioDefault',
  ),
  'has menu' => TRUE,
  'access' => TRUE,

);

class FormioDefault extends FormioActions implements FormioActionsInterface {

  function init($plugin) {
    parent::init($plugin);
  }

  function action($plugin) {

    $args = func_get_args();

    ctools_include('export');
    $schema = ctools_export_get_schema('formio_preset');

    // TODO: Need to know what preset we need here. Replace "export1" with var.
    $export = ctools_export_crud_load('formio_preset', 'export1');

    $data = file_get_contents('php://input');

    $request = formio_rest('form/' . $export->id);
    $results = $request->fetchKeyed('path');

    // We should always have a path but in case we don't throw an error.
    $path = $results->forms['path'];
    if (!isset($path)) {
      // TODO: How do we throw an error here.
      drupal_json_output(json_encode('Example: Could not find a path'));
    }
    $uri = $results->project . '/'. $path . '/submission';

    // This is used to validate the request. This will be posted back to form.io
    // and the result will need to be a 200. Add a ?dryrun=1 query to the post.
    $token = getallheaders()['X-Jwt-Token'];

    // Consider @see drupal_deliver_html_page().
    $result = $this->rest_call($token, $uri, $data);

    if (empty($result['error'])) {
      drupal_json_output(json_encode($result['response']));
    }

    drupal_exit();
  }

  /**
   * Implements hook_menu().
   */
  function hook_menu(&$items) {
    $items['formio-default'] = array(
      'page callback' => $this->plugin['callback'],
      'page arguments' => array($this->plugin['name'], $this->plugin['access']),
      'access callback' => $this->plugin['access'],
      'type' => MENU_CALLBACK,
    );
    parent::hook_menu($items);
  }

  /**
   * Makes a restful call to Form.io.
   *
   * @param string $token
   *   A token used for authentication.
   * @param string $uri
   * @param string $data
   * @param string $method
   *
   * @return array
   */
  function rest_call($token, $uri, $data, $method = 'POST') {
    // Initialize the session.
    $curl = curl_init();

    // Setup options for the transfer.
    curl_setopt_array($curl, array(
      // Adding a query here for a 'Dry Run'.
      CURLOPT_URL => $uri, // . '?dryrun=1',
      CURLOPT_RETURNTRANSFER => TRUE,
      CURLOPT_ENCODING => "",
      CURLOPT_MAXREDIRS => 10,
      CURLOPT_TIMEOUT => 30,
      CURLOPT_HTTP_VERSION => CURL_HTTP_VERSION_1_1,
      CURLOPT_CUSTOMREQUEST => $method,
      CURLOPT_POSTFIELDS => "$data",
      CURLOPT_HTTPHEADER => array(
        "cache-control: no-cache",
        "content-type: application/json",
        "x-jwt-token: " . $token,
      ),
    ));

    // Execute the session.
    $response = curl_exec($curl);

    // The last error, if any, for the current session.
    $error = curl_error($curl);

    // The session result containing the response and any errors.
    $result = array(
      'response' => $response,
      'error' => $error,
    );

    // Close the session.
    curl_close($curl);

    return $result;
  }
}
