@extends('layouts.app')
@php
$ssr = ssr('/js/app-server.js')->render();
@endphp
@push('head.meta')
    {!! $ssr['head'] !!}
    <meta name="description" content="RinPhone.vn - Chuyên kinh doanh trang thiết bị công nghệ di động, Apple, Android,... với giá thành cực kỳ hấp dẫn cho khách hàng đến với RinPhone.vn" />
@endpush
@push('head.css')
    <link href="{{ mix('/css/app.css') }}" rel="stylesheet" id="app-css" />
@endpush
@section('body.content')
    <div id="app">{!! $ssr['app'] !!}</div>
@endsection
@push('body.js')
    <script defer src="{{ mix('/js/app-client.js') }}" id="app-js"></script>
@endpush
