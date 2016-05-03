<?php
/**
 * @file
 * Defines the FormioEntity class.
 */

class FormioEntity extends FormioDefault {

  var $type;
  var $bundle;
  var $title;
  var $uid;

  /**
   * Initialize the FormioEntity class.
   *
   * @param array $plugin
   */
  function init($plugin) {
    global $user;
    $this->uid = $user->uid;
    parent::init($plugin);
  }

  /**
   * Plugin menu callback used to define menu items.
   *
   * @param array $items
   */
  function hook_menu(&$items) {
    parent::hook_menu($items);
  }

  /**
   * The base action for creating an entity.
   *
   * @param array $plugin
   * @param string $preset_name
   * @param object $export
   */
  function action($plugin, $preset_name, $export) {

    $formio = formio_rest('form/' . $export->id);
    $settings = $export->settings;
    $entity_type = $settings['entity_type'];
    $bundle = $settings['entity_bundle'];

    // The form submission data.
    $data = json_decode(file_get_contents('php://input'));

    // Check for a plugin that deals specifically with this entity type.
    list($module) = explode(':', $export->action);
    // Look for a plugin that might want to handle creating this entity type
    // on a more granular level.
    $plugin = ctools_get_plugins($module, 'actions', $entity_type);
    if (!empty($plugin['action'])) {
      $function = $plugin['action'];
      if (function_exists($function)) {
        $success = $function($formio, $data, $export, $bundle, $settings['entity_field_map']);
        if ($success) {
          parent::action($plugin, $preset_name, $export);
        }
      }
    }
    // If we don't find one we'll just do the best we can.
    else {
      $entity = entity_create($entity_type, array('type' => $bundle));
      $entity_wrapper = entity_metadata_wrapper($entity_type, $entity);
      foreach ($settings['entity_field_map'] as $drupal_field => $formio_field) {
        if ($drupal_field == 'title') {
          $entity_wrapper->title = $data->data->{$formio_field};
        }
        else {
          $instance = field_info_instance($entity_type, $drupal_field, $bundle);
          if ($instance) {
            $entity_wrapper->{$instance['field_name']} = array('value' => $data->data->{$formio_field});
          }
        }
      }
      // Save the entity
      if (isset($entity_wrapper)) {
        $entity_wrapper->save();
        parent::action($plugin, $preset_name, $export);
      }
    }
  }
}
