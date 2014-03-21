<?php
/**
 *  uc_shop_intro 
 */
function uc_shop_intro() {
  $output = '';
  global $_domain;
  if ($_domain && $_domain['domain_id']>0) {
    $sql = "SELECT n.nid FROM {node} n INNER JOIN {content_type_shop} c ON n.vid=c.vid
      WHERE c.field_domain_id_value=%d AND n.status>0
      ";
    $result = db_query_range($sql, $_domain['domain_id'], 0, 1);
    $nid = db_result($result);
    $node = node_load($nid);
    $output .= node_page_view($node);
  }
  
  return $output;
}