<?php

/**
 * Base class for the Formio Form plugin.
 */

interface FormioEntityInterface {

}

abstract class FormioEntity implements FormioEntityInterface {

  public $plugin = NULL;

  public $components = array();
  public $componentType = '';
  public $componentKey = '';
  public $componentLabel = '';
  public $componentPlaceholder = '';
  public $componentInputType = '';
  public $componentInput = TRUE;

  public $tags = array();

  function __construct($plugin) {
    $this->plugin = $plugin;
    $this->name = $plugin['name'];
  }

  function types($type = NULL) {
    return entity_get_info($type);
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
}

class FormioNode extends FormioEntity {

  function __construct($plugin, $entity_type) {
    parent::__construct($plugin);
    $this->entity_type = $entity_type;
  }

  function fieldInstances($entity_type, $bundle_name) {
    return parent::fieldInstances('node', $bundle_name);
  }

}
