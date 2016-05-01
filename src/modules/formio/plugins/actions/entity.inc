<?php
/**
 * @file
 * Action plugin to create an entity on form submission.
 */

$plugin = array(
  'title' => t('Create Entity'),

  'handler' => array(
    'class' => 'FormioEntity',
  ),

  'has menu' => FALSE,
  'access' => TRUE,

);

class FormioEntity extends FormioDefault implements FormioActionsInterface {

  function init($plugin) {
    parent::init($plugin);
  }

  function hook_menu(&$items) {
    $items['formio-entity'] = array(
      'page callback' => $this->plugin['callback'],
      'page arguments' => array($this->plugin['name'], $this->plugin['access']),
      'access callback' => $this->plugin['access'],
      'type' => MENU_CALLBACK,
    );
    parent::hook_menu($items);
  }

  function action($plugin) {

    $args = func_get_args();

    ctools_include('export');
    $schema = ctools_export_get_schema('formio_preset');

    // TODO: Need to know what preset we need here. Replace "export1" with var.
    $export = ctools_export_crud_load('formio_preset', 'awesome');

    $data = file_get_contents('php://input');

    $request = formio_rest('form/' . $export->id);
    $results = $request->fetchComponents();

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

  function entityTypes($type = NULL) {
    $types = entity_get_info($type);
    foreach ($types as $name => $entity) {

    }
  }

  function fieldInstances($entity_type = NULL, $bundle_name = NULL) {
    return field_info_instances($entity_type, $bundle_name);
  }

  function fieldInfo($field_name) {
    return field_info_field($field_name);
  }

  function fieldBundles($entity_type = NULL) {
    return field_info_bundles($entity_type);
  }

  function type() {
    return $this->entity_type;
  }
}
