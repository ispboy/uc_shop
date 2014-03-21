<?php
function uc_shop_admincp() {
  $output = '';
  
  global $user;
  global $_domain;
  $user_domains = $user->domain_user;
  $domain_id = $_domain['domain_id'];
  if (array_key_exists($domain_id, $user_domains)) {
    $output .= 'is admin';
  }
  return $output;
}