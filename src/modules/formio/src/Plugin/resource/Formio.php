<?php
/**
 * @file
 *
 * Contails \Drupal\formio\Plugin\resource\Formio
 * Created by PhpStorm.
 * User: randallknutson
 * Date: 5/4/16
 * Time: 11:15 AM
 */

namespace Drupal\formio\Plugin\resource;


use Drupal\restful\Plugin\resource\ResourceEntity;

class Formio extends ResourceEntity
{
  protected function publicFields() {
    $fields = parent::publicFields();

    $instance_fields = field_info_instances($this->entityType, $this->bundles[0]);

    // Map all fields to public.
    foreach($instance_fields as $key => $instance_field) {
      $fields[$key] = array(
        'property' => $key
      );
    }

    return $fields;
  }
}