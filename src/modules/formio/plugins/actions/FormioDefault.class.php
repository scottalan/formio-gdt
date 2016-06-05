<?php
/**
 * @file
 */

class FormioDefault extends FormioActions implements FormioActionsInterface {

  function init($plugin) {
    parent::init($plugin);
  }

  /**
   * Defines the custom menu callback used to submit a form to form.io.
   */
  function hook_menu(&$items) {
    $items['formio/%'] = array(
      'page callback' => $this->plugin['callback'],
      'page arguments' => array($this->plugin['name'], 1),
      'access callback' => $this->plugin['access'],
      'type' => MENU_CALLBACK,
    );
    parent::hook_menu($items);
  }

  /**
   * Defines the action for this plugin.
   *
   * This is triggered by the menu callback.
   */
  function action($plugin, $preset_name, $export) {

    // A json object containing the contents of the form submission.
    $data = file_get_contents('php://input');

    $request = formio_rest('form/' . $export->_id);
    $results = $request->fetchKeyed('path');

    // We should always have a path but in case we don't throw an error.
    $path = $results->result['path'];
    if (!isset($path)) {
      // TODO: Cause an error for $result['error'] and see how it looks.
      drupal_json_output(json_encode('Example: Could not find a path'));
    }

    // The api uri for a submission.
    $uri = $results->project . '/'. $path . '/submission';

    // This is used to validate the request. This will be posted back to form.io
    // and the result will need to be a 200. Add a ?dryrun=1 query to the post.
    $token = getallheaders()['X-Token'];

    // Consider @see drupal_deliver_html_page().
    $result = $this->rest_call($token, $uri, $data);

    if (empty($result['error'])) {
      $output = $result['response'];
    }
    else {
      $output = $result['error'];
    }

    // Encode the output.
    drupal_json_output(json_encode($output));

    // Exit.
    drupal_exit();
  }

  /**
   * Makes a restful call to Form.io.
   *
   * @param string $token
   *   The x-token used for authentication.
   * @param string $uri
   *   The api uri.
   * @param string $data
   *   A json object containing the data submitted in the form.
   *
   * @return array
   *   The result of the rest call.
   */
  private function rest_call($token, $uri, $data) {
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
      CURLOPT_CUSTOMREQUEST => "POST",
      CURLOPT_POSTFIELDS => "$data",
      CURLOPT_HTTPHEADER => array(
        "cache-control: no-cache",
        "content-type: application/json",
        "x-token: " . $token,
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
