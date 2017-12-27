<?php

/**
 * Class ls_draw_views_handler_field_draw_start_button
 */
class quiz_export_views_handler_field_download_link extends views_handler_field {

  function query() {

  }

  function click_sort($order) {

  }

  function render($values) {

    $links = array();

    if (!empty($values->node_type) && $values->node_type == QUIZ_NODE_TYPE) {
      $links[] = quiz_export_download_link($values->nid);
    }

    return implode('<br />', $links);
  }

}
