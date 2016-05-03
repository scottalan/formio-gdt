<?php
/**
 * @file
 * Action plugin to create a node on form submission.
 */

$plugin = array(
  'title' => t('Create Node'),
  'action' => 'formio_node_action',
);

/**
 * The node creation action.
 *
 * @param array $formio
 * @param object $data
 * @param object $export
 * @param string $bundle
 * @param array $field_map
 *
 * @return bool
 *   TRUE if we want to submit the form to form.io.
 */
function formio_node_action($formio, $data, $export, $bundle, $field_map) {
  // Right now this does the same this that FormioEntity()->action() does.
  // @todo: Decide if this is really necessary. Why can't all content type fields
  // @todo: just be text fields? Form.io handles validation on a field level.
  $entity = entity_create('node', array('type' => $bundle));
  $entity_wrapper = entity_metadata_wrapper('node', $entity);
  foreach ($field_map as $drupal_field => $formio_field) {
    if ($drupal_field == 'title') {
      $entity_wrapper->title = $data->data->{$formio_field};
    }
    else {
      $instance = field_info_instance('node', $drupal_field, $bundle);
      if ($instance) {
        $entity_wrapper->{$instance['field_name']} = array('value' => $data->data->{$formio_field});
      }
    }
  }
  // Save the node
  if (isset($entity_wrapper)) {
    $entity_wrapper->save();
    // We want to submit this form to form.io as well.
    return TRUE;
  }
}
