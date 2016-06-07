<?php

namespace Drupal\formio\Plugin\resource\DataProvider;

use Drupal\restful\Plugin\resource\DataProvider\DataProviderEntity;
use Drupal\restful\Plugin\resource\DataProvider\DataProviderInterface;

class DataProviderFormio extends DataProviderEntity implements DataProviderInterface {

  public function entityPreSave(\EntityDrupalWrapper $wrapper) {
    $formio = $wrapper->value();
    if (!empty($formio->sid)) {
      return;
    }
    ctools_include('export');
    $export = ctools_export_crud_load('formio_form', $formio->instance);

    $formio->_id = $export->_id;
    $formio->name = $export->name;
    $formio->created = $formio->changed = REQUEST_TIME;
    $formio->owner = $this->getAccount()->uid;
  }

  public function update($identifier, $object, $replace = FALSE) {
    return parent::update($identifier, $object, $replace);
  }
}
