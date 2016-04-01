<?php

/**
 * Get project forms.
 *
 * @return array
 *   Project forms.
 */
function formio_get_forms($operation = 'GET', $endpoint = 'form', $params = array(), $reset = FALSE) {

  static $drupal_static_fast;
  if (!isset($drupal_static_fast)) {
    $drupal_static_fast = &drupal_static(__FUNCTION__);
  }

  $formio = new FormioRequest();
  $formio->operation($operation);
  $formio->endpoint($endpoint);
  $formio->params($params);
  $formio->request();



  if ($project = variable_get('formio_project_url', NULL)) {
    static $drupal_static_fast;
    if (!isset($drupal_static_fast)) {
      $drupal_static_fast = &drupal_static(__FUNCTION__);
    }
    $forms = &$drupal_static_fast;
    if ($reset || !isset($forms)) {
      if (!$reset && $cache = cache_get('formio_forms_' . $hook)) {
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
        switch ($hook) {
          case 'all':
            foreach ($data as $key => $form) {
              if ($form['type'] == 'form') {
                $forms[$form['name']] = array('path' => $form['path'], 'title' => $form['title']);
              }
            }
          break;

          default:
            foreach ($data as $key => $form) {
              if ($form['type'] == 'form' && $form['name'] == $hook) {
                $forms[$form['name']] = $form;
              }
            }
            break;

        }
        // Store our calculated data in the cache.
        cache_set('formio_forms_' . $hook, $forms);
      }
    }
    return $forms;
  }
}

function formio_get_options($data) {
  foreach ($data as $key => $form) {
    if ($form['type'] == 'form') {
      $forms[$form['name']] = array('path' => $form['path'], 'title' => $form['title']);
    }
  }
}

function formio_get_all($data) {
  foreach ($data as $key => $form) {
    if ($form['type'] == 'form') {
      $forms[$form['name']] = $form;
    }
  }
}

function formio_get_by_name($data, $name) {
  foreach ($data as $key => $form) {
    if ($form['type'] == 'form' && $form['name'] == $name) {
      $forms[$form['name']] = $form;
    }
  }
}
