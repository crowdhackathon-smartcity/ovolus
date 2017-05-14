<?php

namespace Drupal\service_collector\Tests\Unit;

/**
 * Service Collector Tests.
 */
class ServiceCollectorUnitTest extends \DrupalWebTestCase {

  /**
   * {@inheritdoc}
   */
  public static function getInfo() {
    return [
      'name' => 'Service Collector Tests',
      'group' => 'service_collector',
    ];
  }

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp('service_collector_test');
  }

  /**
   * Ensure hooks works properly.
   */
  public function testNumeric() {
    $this->assertEqual(service_collector_numeric(), [
      'one' => 1,
      'two' => 22,
    ]);
  }

}
