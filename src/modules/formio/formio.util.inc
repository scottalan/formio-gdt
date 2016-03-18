<?php

/**
 * HACK: Gets token to allow hitting the API.
 *
 * @todo: Temporary
 */
function formio_token() {
  // If this doesn't work go and get a new one. (May Expire).
  $token = file_get_contents(drupal_get_path('module', 'formio') . '/token');
  if (!isset($token) || empty($token)) {
    drupal_set_message(t('You need a token file!'), 'error');
  }
  return file_get_contents(drupal_get_path('module', 'formio') . '/token');
}

/**
 * HACK: Builds a header for API access.
 *
 * @todo: Temporary.
 */
function formio_header() {
  $token = formio_token();
  return array('x-jwt-token' => $token);
}

/**
 * Gets the api url.
 *
 * @return string|null
 *   The API url.
 */
function formio_get_project() {
  return variable_get('formio_project_url', NULL);
}

/**
 * Loads JS and CSS for Formio.
 *
 * @todo: Temporary.
 */
function formio_load_assets() {
  drupal_add_js('https://cdnjs.cloudflare.com/ajax/libs/angular.js/1.5.0/angular.js', array(
      'type' => 'external',
      'group' => JS_THEME,
      'weight' => -1,
    )
  );
  drupal_add_js(drupal_get_path('module', 'formio') . '/js/formio-complete.min.js', array(
      'group' => JS_THEME,
      'weight' => 2,
    )
  );
  drupal_add_js(drupal_get_path('module', 'formio') . '/js/formio.js', array(
    'group' => JS_THEME,
    'weight' => 10,
  ));
  drupal_add_css(drupal_get_path('module', 'formio') . '/css/formio-complete.min.css');
  drupal_add_css(drupal_get_path('module', 'formio') . '/css/formio.css');
}

/**
 * Get project forms.
 *
 * @return array
 *   Project forms.
 */
function formio_get_forms() {
  if ($project = formio_get_project()) {
    static $drupal_static_fast;
    if (!isset($drupal_static_fast)) {
      $drupal_static_fast = &drupal_static(__FUNCTION__);
    }
    $forms = &$drupal_static_fast;
    if (!isset($forms)) {
      if ($cache = cache_get('formio_forms')) {
        // Load the data straight from the cache.
        $forms = $cache->data;
      }
      else {
        $errors = FALSE;
        $options = array(
          'headers' => formio_header(),
        );
        $request = drupal_http_request($project . '/form', $options);
        if ($request->code != 200) {
          if ($errors) {

          }
          return FALSE;
        }
        $data = drupal_json_decode($request->data);
        if (!$data) {
          if ($errors) {

          }
          return FALSE;
        }
        foreach ($data as $form) {
          if ($form['type'] == 'form') {
            $forms[$form['name']] = array('path' => $form['path'], 'title' => $form['title']);
          }
        }
        // Store our calculated data in the cache.
        cache_set('formio_forms', $forms);
      }
    }
    return $forms;
  }
}
